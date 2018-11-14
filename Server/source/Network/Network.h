#pragma once
#include "enet/enet.h"
#include "Common/Platform.h"
#include "Network/NetInterface.h"
#include "Common/Protocol/Protocol.h"

class Network
{
public:
	Network();
	virtual ~Network();
	bool Init(uint16_t uPort);
	void Start();

public:
	void SetNetInterface(NetInterface* poNetInterface);
	bool SendMsg(ENetPeer* poENetPeer, int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto);

private:
	ENetHost* m_poEnetServer;
	uint16_t m_uListenPort;
	NetInterface* m_poNetInterface;
};
