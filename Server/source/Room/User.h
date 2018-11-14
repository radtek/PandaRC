#pragma once
#include "enet/enet.h"
#include "Common/Platform.h"

class User
{
public:
	typedef std::list<int> RoomList;
	typedef RoomList::iterator RoomIter;

public:
	User();
	virtual ~User() {};

	std::string& GetMacAddr() { return m_oMacAddr; }
	void SetMacAddr(const std::string& oMacAddr) { m_oMacAddr = oMacAddr; }

	ENetPeer* GetENetPeer() { return m_poENetPeer; }
	void SetEnetPeer(ENetPeer* poENetPeer) { m_poENetPeer = poENetPeer; }

	RoomList& GetRoomList() { return m_oRoomList; }

private:
	ENetPeer* m_poENetPeer;
	std::string m_oMacAddr;

	RoomList m_oRoomList;
};
