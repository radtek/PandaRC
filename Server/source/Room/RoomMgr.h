#pragma once

#include "Common/Network/NetInterface.h"
#include "Common/Platform.h"
#include "Room/Room.h"
#include "Room/User.h"

class RoomMgr : public NetInterface
{
public:
	typedef std::unordered_map<int, Room*> RoomMap;
	typedef RoomMap::iterator RoomIter;

	typedef std::unordered_map<int, User*> UserMap;
	typedef UserMap::iterator UserIter;

public:
	RoomMgr();
	virtual ~RoomMgr();

	Room* CreateRoom();
	Room* GetRoom(int nRoomID);
	void RemoveRoom(int nRoomID);

	User* CreateUser(int nUserID);
	User* GetUser(int nUserID);
	void RemoveUser(int nuserID);

public:
	virtual void OnConnect(ENetEvent& event);
	virtual void OnReceive(ENetEvent& event);
	virtual void OnDisconnect(ENetEvent& event);

protected:
	int GenRoomID();

private:
	int m_nAutoRoomID;
	RoomMap m_oRoomMap;
	UserMap m_oUserMap;

};
