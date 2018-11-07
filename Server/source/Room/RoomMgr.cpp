#include "RoomMgr.h"

RoomMgr::RoomMgr()
{
	m_nAutoID = 0;
}

RoomMgr::~RoomMgr()
{
	RoomIter iter = m_oRoomMap.begin();
	for (iter; iter != m_oRoomMap.end(); iter++)
	{
		delete iter->second;
	}
	m_oRoomMap.clear();
}

Room* RoomMgr::CreateRoom()
{
	int nRoomID = GenID();
	Room* poRoom = new Room(nRoomID);
	m_oRoomMap[nRoomID] = poRoom;
	return poRoom;
}

Room* RoomMgr::GetRoom(int nID)
{
	RoomIter iter = m_oRoomMap.find(nID);
	if (iter != m_oRoomMap.end())
	{
		return iter->second;
	}
	return NULL;
}
		
void RoomMgr::RemoveRoom(int nID)
{
	RoomIter iter = m_oRoomMap.find(nID);
	if (iter != m_oRoomMap.end())
	{
		delete iter->second;
		m_oRoomMap.erase(iter);
	}
}

int RoomMgr::GenID()
{
	m_nAutoID = (m_nAutoID % 0x7FFFFFFF) + 1;
	return m_nAutoID;
}

