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
	enet_deinitialize();
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
}

bool Network::Init()
{
	if (enet_initialize() != 0)
	{
		XLog(LEVEL_ERROR,"An error occurred while initializing ENet.\n");
		return false;
	}
	return true;
}

void Network::SetNetInterface(NetInterface* poNetInterface)
{
	m_poNetInterface = poNetInterface;
}

bool Network::Listen(uint16_t uPort)
{
	if (m_poEnetServer != NULL)
	{
		return true;
	}

	m_uListenPort = uPort;
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = m_uListenPort;

	m_poEnetServer = enet_host_create(&address, 1024, 2, 0, 0);
	if (m_poEnetServer == NULL)
	{
		XLog(LEVEL_ERROR, "An error occurred while trying to create an ENet server host.\n");
		return false;
	}
	XLog(LEVEL_DEBUG, "Listen at port:%d\n", uPort);
	return true;
}

bool Network::Connect(const std::string& oIP, uint16_t uPort)
{
	if (m_poEnetClient == NULL)
	{
		m_poEnetClient = enet_host_create(NULL, 1, 2, 0, 0);
		if (m_poEnetClient == NULL)
		{
			XLog(LEVEL_ERROR, "An error occurred while trying to create an ENet client host.\n");
			return false;
		}
	}

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
		XLog(LEVEL_INFO, "Connection to %s:%d succeeded.\n", oIP.c_str(), uPort);
		return true;
	}
	enet_peer_reset(m_poClientPeer);
	m_poClientPeer = NULL;
	XLog(LEVEL_ERROR, "Connection to %s:%d failed.\n", oIP.c_str(), uPort);
	return false;
}

void Network::CloseServerPeer(ENetPeer* poENetPeer)
{
	enet_peer_disconnect(poENetPeer, 0);
}

void Network::CloseClientPeer()
{
	if (m_poClientPeer == NULL)
	{
		return;
	}
	enet_peer_disconnect(m_poClientPeer, 0);
}

bool Network::Send2Server(int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto)
{
	if (m_poClientPeer == NULL || m_poEnetClient == NULL)
	{
		return false;
	}
	ENetPacket* poPacket = enet_packet_create((void*)poProto, poProto->size, nPacketFlag);
	enet_peer_send(m_poClientPeer, 0, poPacket);
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
	enet_peer_send(poENetPeer, 0, poPacket);
	enet_host_flush(m_poEnetServer);
	return true;
}

void Network::Start()
{
	if (m_poEnetClient == NULL && m_poEnetServer == NULL)
	{
		XLog(LEVEL_INFO, "Not make client or server net.\n");
		return;
	}
	for (;;)
	{
		ENetEvent event;
		if (m_poEnetServer != NULL)
		{
			while (enet_host_service(m_poEnetServer, &event, 10) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					XLog(LEVEL_INFO, "A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
					/* Store any relevant client information here. */
					if (m_poNetInterface != NULL)
					{
						event.peer->data = NULL;
						m_poNetInterface->OnConnect(event);
					}
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					XLog(LEVEL_INFO, "A packet of length %u containing %s was received from %s on channel %u.\n", event.packet->dataLength, event.packet->data, event.peer->data, event.channelID);
					/* Clean up the packet now that we're done using it. */
					if (m_poNetInterface != NULL)
					{
						m_poNetInterface->OnReceive(event);
					}
					//enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					XLog(LEVEL_INFO, "%s disconnected.\n", event.peer->data);
					/* Reset the peer's client information. */
					if (m_poNetInterface != NULL)
					{
						m_poNetInterface->OnDisconnect(event);
					}
					event.peer->data = NULL;
					break;
				}
			}
		}
		if (m_poEnetClient != NULL)
		{
			int nMSTime = m_poEnetServer != NULL ? 0 : 10;
			while (enet_host_service(m_poEnetClient, &event, nMSTime) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					XLog(LEVEL_INFO, "A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
					/* Store any relevant client information here. */
					if (m_poNetInterface != NULL)
					{
						event.peer->data = NULL;
						m_poNetInterface->OnConnect(event);
					}
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					XLog(LEVEL_INFO, "A packet of length %u containing %s was received from %s on channel %u.\n", event.packet->dataLength, event.packet->data, event.peer->data, event.channelID);
					/* Clean up the packet now that we're done using it. */
					if (m_poNetInterface != NULL)
					{
						m_poNetInterface->OnReceive(event);
					}
					//enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					XLog(LEVEL_INFO, "%s disconnected.\n", event.peer->data);
					/* Reset the peer's client information. */
					if (m_poNetInterface != NULL)
					{
						m_poNetInterface->OnDisconnect(event);
					}
					event.peer->data = NULL;
					break;
				}
			}
		}
	}
}
