#pragma once

#include "Room.h"
#include <unordered_map>

class RoomMgr
{
public:
	typedef std::unordered_map<int, Room*> RoomMap;
	typedef RoomMap::iterator RoomIter;

public:
	RoomMgr();
	virtual ~RoomMgr();

	Room* CreateRoom();
	Room* GetRoom(int nID);
	void RemoveRoom(int nID);

protected:
	int GenID();

private:
	int m_nAutoID;
	RoomMap m_oRoomMap;
};
