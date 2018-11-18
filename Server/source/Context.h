#pragma once
#include "Room/RoomMgr.h"
#include "Server.h"

struct Context
{
	Server* poServer;
	RoomMgr* poRoomMgr;
};

extern Context* gpoContext;