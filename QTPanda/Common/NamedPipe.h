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
	bool Init(HWND hWnd);
	MailBox<NSPROTO::COM_DATA>& GetMailBox() { return m_oMailBox; }
	bool SendMsg2Client(DWORD dwProcessID, NSPROTO::PROTO* data);

protected:
	static void AcceptThread(void* pParam);
	static void ServerReadThread(void* pParam);

	HANDLE GetProcessPipe(DWORD dwProcessID)
	{
		std::map<DWORD, HANDLE>::iterator iter =  m_oProcessPipeMap.find(dwProcessID);
		if (iter == m_oProcessPipeMap.end())
		{
			return NULL;
		}
		return iter->second;
	}
	void AddProcessPipe(DWORD dwProcessID, HANDLE hPipe)
	{
		m_oMutex.Lock();
		if (GetProcessPipe(dwProcessID) != NULL)
		{
			OutputDebugString("Process allready register\n");
		}
		else
		{
			m_oProcessPipeMap[dwProcessID] = hPipe;
		}
		m_oMutex.Unlock();
	}
	void RemoveProcessPipe(DWORD dwProcessID)
	{
		m_oMutex.Lock();
		m_oProcessPipeMap.erase(dwProcessID);
		m_oMutex.Unlock();
	}
	void RemovePipeProcess(HANDLE hPipe)
	{
		m_oMutex.Lock();
		std::map<DWORD, HANDLE>::iterator iter = m_oProcessPipeMap.begin();
		for (; iter != m_oProcessPipeMap.end(); iter++)
		{
			if (iter->second == hPipe) {
				m_oProcessPipeMap.erase(iter);
				break;
			}
		}
		m_oMutex.Unlock();
	}

private:
	HWND m_hWnd;
	XThread* m_pAcceptThread;
	MailBox<NSPROTO::COM_DATA> m_oMailBox;

	MutexLock m_oMutex;
	std::map<DWORD, HANDLE> m_oProcessPipeMap;
};

//CLIENT
class NamedPipeClient
{
public:
	NamedPipeClient();
	bool Init();
	bool SendMsg2Server(NSPROTO::PROTO* data);
	void ShutDown() { m_bShutDown = true; }
	bool IsShutDown() { return m_bShutDown; }

protected:
	static void ClientReadThread(void* pParam);

private:
	HANDLE m_hPipe;
	bool m_bShutDown;
};
