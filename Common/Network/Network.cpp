#include "Common/Network/Network.h"
#include "Include/Logger/Logger.hpp"

Network::Network()
{
	m_uListenPort = -1;
	m_poNetInterface = NULL;
	m_poEnetServer = NULL;
	m_poEnetClient = NULL;
	m_poClientPeer = NULL;
}

Network::~Network()
{
	if (m_poEnetServer != NULL)
	{
		enet_host_destroy(m_poEnetServer);
		m_poEnetServer = NULL;
	}
	if (m_poClientPeer != NULL)
	{
		enet_peer_reset(m_poClientPeer);
		m_poClientPeer = NULL;
	}
	if (m_poEnetClient != NULL)
	{
		enet_host_destroy(m_poEnetClient);
		m_poEnetClient = NULL;
	}
	enet_deinitialize();
}

bool Network::InitAsServer(uint16_t uListenPort)
{
	if (m_poEnetServer != NULL)
	{
		return true;
	}
	if (enet_initialize() != 0)
	{
		XLog(LEVEL_ERROR,"An error occurred while initializing ENet.\n");
		return false;
	}
	m_uListenPort = uListenPort;
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = m_uListenPort;

	m_poEnetServer = enet_host_create(&address, 1024, 2, 0, 0);
	if (m_poEnetServer == NULL)
	{
		XLog(LEVEL_ERROR, "An error occurred while trying to create an ENet server host.\n");
		return false;
	}
	enet_host_compress_with_range_coder(m_poEnetServer);
	XLog(LEVEL_DEBUG, "Listen at port:%d\n", uListenPort);
	return true;
}

bool Network::InitAsClient()
{
	if (m_poEnetClient != NULL)
	{
		return true;
	}
	if (enet_initialize() != 0)
	{
		XLog(LEVEL_ERROR,"An error occurred while initializing ENet.\n");
		return false;
	}
	m_poEnetClient = enet_host_create(NULL, 1, 2, 0, 0);
	if (m_poEnetClient == NULL)
	{
		XLog(LEVEL_ERROR, "An error occurred while trying to create an ENet client host.\n");
		return false;
	}
	enet_host_compress_with_range_coder(m_poEnetClient);
	return true;
}

void Network::SetNetInterface(NetInterface* poNetInterface)
{
	m_poNetInterface = poNetInterface;
}

bool Network::Connect(const std::string& oIP, uint16_t uPort)
{
	CloseClientPeer();

	ENetEvent event;
	ENetAddress address;
	enet_address_set_host(&address, oIP.c_str());
	address.port = uPort;

	m_poClientPeer = enet_host_connect(m_poEnetClient, &address, 2, 0);
	if (m_poClientPeer == NULL)
	{
		XLog(LEVEL_ERROR, "No available peers for initiating an ENet connection.\n");
		return false;
	}
	if (enet_host_service(m_poEnetClient, &event, 3000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		XLog(LEVEL_INFO, "Connection to %s:%d successful\n", oIP.c_str(), uPort);
		if (m_poNetInterface != NULL)
		{
			m_poNetInterface->OnConnect(event);
		}
		return true;
	}
	enet_peer_reset(m_poClientPeer);
	m_poClientPeer = NULL;
	XLog(LEVEL_ERROR, "Connection to %s:%d failed.\n", oIP.c_str(), uPort);
	return false;
}

void Network::CloseServerPeer(ENetPeer* poENetPeer)
{
	enet_peer_reset(poENetPeer);
}

void Network::CloseClientPeer()
{
	if (m_poClientPeer == NULL)
	{
		return;
	}
	enet_peer_reset(m_poClientPeer);
	m_poClientPeer = NULL;
}

bool Network::Send2Server(int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto)
{
	if (m_poClientPeer == NULL || m_poEnetClient == NULL)
	{
		return false;
	}
	ENetPacket* poPacket = enet_packet_create((void*)poProto, poProto->size, nPacketFlag);
	enet_peer_send(m_poClientPeer, nChannel, poPacket);
	enet_host_flush(m_poEnetClient);
	return true;
}

bool Network::Send2ServerRaw(int nChannel, int nPacketFlag, uint8_t* pData, int nSize)
{

	if (m_poClientPeer == NULL || m_poEnetClient == NULL)
	{
		return false;
	}
	ENetPacket* poPacket = enet_packet_create((void*)pData, nSize, nPacketFlag);
	enet_peer_send(m_poClientPeer, nChannel, poPacket);
	enet_host_flush(m_poEnetClient);
	return true;
}

bool Network::Send2Client(ENetPeer* poENetPeer, int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto)
{
	if (m_poEnetServer == NULL || poENetPeer == NULL)
	{
		return false;
	}
	ENetPacket* poPacket = enet_packet_create((void*)poProto, poProto->size, nPacketFlag);
	enet_peer_send(poENetPeer, nChannel, poPacket);
	enet_host_flush(m_poEnetServer);
	return true;
}

bool Network::Send2ClientRaw(ENetPeer* poENetPeer, int nChannel, int nPacketFlag, uint8_t* pData, int nSize)
{
	if (m_poEnetServer == NULL || poENetPeer == NULL)
	{
		return false;
	}
	ENetPacket* poPacket = enet_packet_create((void*)pData, nSize, nPacketFlag);
	enet_peer_send(poENetPeer, nChannel, poPacket);
	enet_host_flush(m_poEnetServer);
	return true;
}

void Network::Update(uint32_t nMSTime)
{
	if (m_poEnetClient == NULL && m_poEnetServer == NULL)
	{
		XLog(LEVEL_INFO, "Not client or server net.\n");
		return;
	}

	ENetEvent event;
	if (m_poEnetServer != NULL)
	{
		while (enet_host_service(m_poEnetServer, &event, nMSTime) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				XLog(LEVEL_INFO, "Server: a new client from %d:%u.\n", event.peer->address.host, event.peer->address.port);
				/* Store any relevant client information here. */
				if (m_poNetInterface != NULL)
				{
					event.peer->data = NULL;
					m_poNetInterface->OnConnect(event);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				XLog(LEVEL_INFO, "Server: a packet of length %u was received from 0x%x on channel:%u.\n", event.packet->dataLength, event.peer->data, event.channelID);
				/* Clean up the packet now that we're done using it. */
				if (m_poNetInterface != NULL)
				{
					m_poNetInterface->OnReceive(event);
				}
				//enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				XLog(LEVEL_INFO, "Server: 0x%x disconnected\n", event.peer->data);
				/* Reset the peer's client information. */
				if (m_poNetInterface != NULL)
				{
					m_poNetInterface->OnDisconnect(event);
				}
				event.peer->data = NULL;
				enet_peer_reset(event.peer);
				break;
			}
		}
	}
	if (m_poEnetClient != NULL)
	{
		nMSTime = m_poEnetServer != NULL ? 0 : nMSTime;
		while (enet_host_service(m_poEnetClient, &event, nMSTime) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				XLog(LEVEL_INFO, "Client: a new client from %u:%u\n", event.peer->address.host, event.peer->address.port);
				/* Store any relevant client information here. */
				if (m_poNetInterface != NULL)
				{
					event.peer->data = NULL;
					m_poNetInterface->OnConnect(event);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				XLog(LEVEL_INFO, "Client: a packet of length %u was received from 0x%x on channel:%u\n", event.packet->dataLength, event.peer->data, event.channelID);
				/* Clean up the packet now that we're done using it. */
				if (m_poNetInterface != NULL)
				{
					m_poNetInterface->OnReceive(event);
				}
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				XLog(LEVEL_INFO, "Client: 0x%x disconnected\n", event.peer->data);
				/* Reset the peer's client information. */
				if (m_poNetInterface != NULL)
				{
					m_poNetInterface->OnDisconnect(event);
				}
				event.peer->data = NULL;
				enet_peer_reset(event.peer);
				break;
			}
		}
	}
}
