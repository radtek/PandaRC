#include "Room/RoomMgr.h"
#include "Include/Logger/Logger.hpp"
#include "Network/Protocol.h"

RoomMgr::RoomMgr()
{
	m_nAutoRoomID = 0;
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

int RoomMgr::GenRoomID()
{
	m_nAutoRoomID = (m_nAutoRoomID % 0x7FFFFFFF) + 1;
	return m_nAutoRoomID;
}

Room* RoomMgr::CreateRoom()
{
	int nRoomID = GenRoomID();
	Room* poRoom = new Room(nRoomID);
	m_oRoomMap[nRoomID] = poRoom;
	return poRoom;
}

Room* RoomMgr::GetRoom(int nRoomID)
{
	RoomIter iter = m_oRoomMap.find(nRoomID);
	if (iter != m_oRoomMap.end())
	{
		return iter->second;
	}
	return NULL;
}
		
void RoomMgr::RemoveRoom(int nRoomID)
{
	RoomIter iter = m_oRoomMap.find(nRoomID);
	if (iter != m_oRoomMap.end())
	{
		delete iter->second;
		m_oRoomMap.erase(iter);
	}
}


User* RoomMgr::CreateUser(const std::string& oMacAddr)
{
	User* poUser = new User();
	m_oUserMap[oMacAddr] = poUser;
	return poUser;
}

User* RoomMgr::GetUser(const std::string& oMacAddr)
{
	UserIter iter = m_oUserMap.find(oMacAddr);
	if (iter != m_oUserMap.end())
	{
		return iter->second;
	}
	return NULL;
}

void RoomMgr::RemoveUser(const std::string& oMacAddr)
{
	UserIter iter = m_oUserMap.find(oMacAddr);
	if (iter != m_oUserMap.end())
	{
		delete iter->second;
		m_oUserMap.erase(iter);
	}
}




void RoomMgr::OnConnect(ENetEvent event)
{

}

void RoomMgr::OnReceive(ENetEvent event)
{
	if (event.packet->dataLength < sizeof(PROTO::HEAD))
	{
		enet_packet_destroy(event.packet);
		XLog(LEVEL_ERROR, "Invalid packet error\n");
		return;
	}

	PROTO::HEAD head = *(PROTO::HEAD*)event.packet;
	if (event.packet->dataLength != head.size)
	{
		enet_packet_destroy(event.packet);
		XLog(LEVEL_ERROR, "Packet size error cmd:%d size:%d\n", head.cmd, head.size);
		return;
	}

	switch (head.cmd)
	{
	case NETCMD::eLOGIN:
		PROTO::LOGIN login = *(PROTO::LOGIN*)event.packet;
		if (GetUser(login.mac_addr) == NULL)
		{
			User* poUser = CreateUser(login.mac_addr);
			poUser->SetMacAddr(login.mac_addr);
			event.peer->data = (void*)poUser;
		}
		else
		{
			XLog(LEVEL_ERROR, "Client %s allready login\n", login.mac_addr);
		}
		enet_packet_destroy(event.packet);
		break;
	case NETCMD::eBUILD:
		PROTO::BUILD build = *(PROTO::BUILD*)event.packet;
		User* poUserClient = GetUser(build.mac_client);
		User* poUserServer = GetUser(build.mac_server);
		if (poUserClient == NULL || poUserServer == NULL)
		{
			XLog(LEVEL_ERROR, "Build error client:0x%u server:0x%u\n", (void*)poUserClient, (void*)poUserServer);
		}
		Room* poRoom = CreateRoom();
		poRoom->SetBuildMac(build.mac_client, build.mac_server);
		enet_packet_destroy(event.packet);
		break;
	case NETCMD::eUNBUILD:
		enet_packet_destroy(event.packet);
		break;
	default:
	}
}

void RoomMgr::OnDisconnect(ENetEvent event)
{

}
