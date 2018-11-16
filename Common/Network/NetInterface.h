#pragma once
#include "enet/enet.h"

class NetInterface
{
public:
	virtual void OnConnect(ENetEvent& event) {};
	virtual void OnReceive(ENetEvent& event) {};
	virtual void OnDisconnect(ENetEvent& event) {};

public:
	
};
