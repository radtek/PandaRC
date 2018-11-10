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
	m_oWorker.Create(Network::WorkerThread, this);
	return true;
}

void Network::SetNetInterface(NetInterface* poNetInterface)
{
	m_poNetInterface = poNetInterface;
}

void Network::WorkerThread(void* param)
{
	Network* poNetwork = (Network*)param;

	for (;;)
	{
		ENetEvent event;
		while (enet_host_service(poNetwork->m_poEnetServer, &event, 10) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				XLog(LEVEL_INFO, "A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
				/* Store any relevant client information here. */
				if (poNetwork->m_poNetInterface != NULL)
				{
					event.peer->data = "Client information";
					poNetwork->m_poNetInterface->OnConnect(event);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				XLog(LEVEL_INFO, "A packet of length %u containing %s was received from %s on channel %u.\n", event.packet->dataLength, event.packet->data, event.peer->data, event.channelID);
				/* Clean up the packet now that we're done using it. */
				if (poNetwork->m_poNetInterface != NULL)
				{
					poNetwork->m_poNetInterface->OnReceive(event);
				}
				//enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				XLog(LEVEL_INFO, "%s disconnected.\n", event.peer->data);
				/* Reset the peer's client information. */
				if (poNetwork->m_poNetInterface != NULL)
				{
					poNetwork->m_poNetInterface->OnDisconnect(event);
				}
				event.peer->data = NULL;
				break;
			}
		}
	}
}
