#pragma once
#include "enet/enet.h"
#include "Common/Network/NetInterface.h"
#include "Common/Platform.h"
#include "Common/Protocol/Protocol.h"

class Network
{
public:
	Network();
	virtual ~Network();

	bool Init();
	bool Listen(uint16_t uPort);
	bool Connect(const std::string& oIP, uint16_t uPort);

	void Start();

public:
	void SetNetInterface(NetInterface* poNetInterface);
	bool Send2Server(int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto);
	bool Send2Client(ENetPeer* poENetPeer, int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto);
	void CloseServerPeer(ENetPeer* poENetPeer);
	void CloseClientPeer();

private:
	uint16_t m_uListenPort;
	ENetHost* m_poEnetServer;

	ENetHost* m_poEnetClient;
	ENetPeer *m_poClientPeer;
	NetInterface* m_poNetInterface;
};
