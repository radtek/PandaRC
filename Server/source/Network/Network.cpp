#include "Network/Network.h"
#include "Include/Logger/Logger.hpp"

Network::Network()
{
	m_uListenPort = -1;
	m_poNetInterface = NULL;
}

Network::~Network()
{
	enet_deinitialize();
	if (m_poEnetServer != NULL)
	{
		enet_host_destroy(m_poEnetServer);
	}
}

bool Network::Init(uint16_t uPort)
{
	m_uListenPort = uPort;
	if (enet_initialize() != 0)
	{
		XLog(LEVEL_ERROR,"An error occurred while initializing ENet.\n");
		return false;
	}
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = m_uListenPort;

	m_poEnetServer = enet_host_create(&address, 1024, 2, 0, 0);
	if (m_poEnetServer == NULL)
	{
		XLog(LEVEL_ERROR, "An error occurred while trying to create an ENet server host.\n");
		return false;
	}
	return true;
}

void Network::SetNetInterface(NetInterface* poNetInterface)
{
	m_poNetInterface = poNetInterface;
}

bool Network::SendMsg(ENetPeer* poENetPeer, int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto)
{
	if (m_poEnetServer == NULL)
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
	for (;;)
	{
		ENetEvent event;
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
}
