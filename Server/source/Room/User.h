#pragma once
#include "enet/enet.h"
#include "Common/Platform.h"

class User
{
public:
	typedef std::list<int> RoomList;
	typedef RoomList::iterator RoomIter;

public:
	User(int nID);
	virtual ~User() {};

	int GetID() { return m_nID; }
	static int GenID();

	std::string& GetMacAddr() { return m_oMacAddr; }
	void SetMacAddr(const std::string& oMacAddr) { m_oMacAddr = oMacAddr; }

	ENetPeer* GetENetPeer() { return m_poENetPeer; }
	void SetEnetPeer(ENetPeer* poENetPeer) { m_poENetPeer = poENetPeer; }

	RoomList& GetClientRoomList() { return m_oClientRoomList; }
	void RemoveClientRoomID(int nRoomID);

private:
	int m_nID;
	std::string m_oMacAddr;
	ENetPeer* m_poENetPeer;
	RoomList m_oClientRoomList;
};
