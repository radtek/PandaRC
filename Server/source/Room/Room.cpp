#include "Room/Room.h"

Room::Room(int nRoomID)
{
	m_nRoomID = nRoomID;
}

Room::~Room()
{
}

void Room::SetBuildMac(const std::string& oClientMac, const std::string& oServerMac)
{
	m_oClientMac = oClientMac;
	m_oServerMac = oServerMac;
}
