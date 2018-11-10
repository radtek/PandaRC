#pragma once
#include "enet/enet.h"
#include "Common/DataStruct/Thread.h"
#include "Network/NetInterface.h"

#include <stdint.h>

class Network
{
public:
	Network();
	virtual ~Network();
	bool Init(uint16_t uPort);
	void SetNetInterface(NetInterface* poNetInterface);

protected:
	static void WorkerThread(void* param);

private:
	ENetHost* m_poEnetServer;
	uint16_t m_uListenPort;

	NetInterface* m_poNetInterface;
	MThread m_oWorker;
};
