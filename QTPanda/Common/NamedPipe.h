#pragma once
#include <string>
#include "Common/MailBox.h"
#include "Common/MsgDef.h"
#include "Common/XThread.h"

//SERVER
class NamedPipeServer
{
public:
	NamedPipeServer();
	bool Init();

protected:
	static void AcceptThread(void* pParam);
	static void ServerReadThread(void* pParam);

public:
	bool SendMsg2Client(HANDLE hPipe, NSPROTO::DATA* data);

private:
	XThread* m_pAcceptThread;
	MailBox<NSPROTO::DATA> m_oMailBox;
};

//CLIENT
class NamedPipeClient
{
public:
	NamedPipeClient();
	bool Init();
	bool SendMsg2Server(NSPROTO::DATA* data);
	void ShutDown() { m_bShutDown = true; }

protected:
	static void ClientReadThread(void* pParam);

private:
	HANDLE m_hPipe;
	bool m_bShutDown;
};
