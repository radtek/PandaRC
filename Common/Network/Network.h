#pragma once
#include "enet/enet.h"
#include "Common/Network/Protocol.h"
#include "Common/Network/NetInterface.h"
#include "Common/Platform.h"

class Network
{
public:
	Network();
	virtual ~Network();

	bool InitAsServer(uint16_t uListenPort);
	bool InitAsClient();
	bool Connect(const std::string& oIP, uint16_t uPort);
	void Update(uint32_t nMSTime);

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
