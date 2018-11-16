#pragma once
#include "Common/Network/Network.h"
#include "Room/RoomMgr.h"

struct Context
{
	Network* poNetwork;
	RoomMgr* poRoomMgr;
};

extern Context* gpoContext;