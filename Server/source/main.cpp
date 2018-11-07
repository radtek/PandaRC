#include "enet/enet.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		exit(EXIT_FAILURE);
	}
	atexit(enet_deinitialize);

	ENetAddress address;
	ENetHost* server;
	address.host = ENET_HOST_ANY;
	address.port = 10001;
	server = enet_host_create(&address, 32, 2, 0, 0);
	if (server == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "create an ENet server host sucessful.\n");

	for (;;)
	{
		ENetEvent event;
		while (enet_host_service(server, &event, 100) > 0)
		{
			fprintf(stdout, "%d***************\n", event.type);
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				fprintf(stdout, "A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
				/* Store any relevant client information here. */
				event.peer->data = "Client information";
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				fprintf(stdout, "A packet of length %u containing %s was received from %s on channel %u.\n", event.packet->dataLength, event.packet->data, event.peer->data, event.channelID);
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				fprintf(stdout, "%s disconnected.\n", event.peer->data);
				/* Reset the peer's client information. */
				event.peer->data = NULL;
				break;
			}
		}
	}

	enet_host_destroy(server);
	return 0;
}