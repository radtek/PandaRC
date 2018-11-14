#include "Room/RoomMgr.h"
#include "Include/Logger/Logger.hpp"
#include "Context.h"
#include "Network/Network.h"

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




void RoomMgr::OnConnect(ENetEvent& event)
{

}

void RoomMgr::OnReceive(ENetEvent& event)
{
	if (event.packet->dataLength < sizeof(NSPROTO::HEAD))
	{
		enet_packet_destroy(event.packet);
		XLog(LEVEL_ERROR, "Invalid packet error\n");
		return;
	}

	NSPROTO::HEAD head = *(NSPROTO::HEAD*)event.packet;
	if (event.packet->dataLength != head.size)
	{
		enet_packet_destroy(event.packet);
		XLog(LEVEL_ERROR, "Packet size error cmd:%d size:%d\n", head.cmd, head.size);
		return;
	}

	switch (head.cmd)
	{
	case NSNETCMD::eLOGIN_REQ:
	{
		NSPROTO::LOGIN_REQ login = *(NSPROTO::LOGIN_REQ*)event.packet;
		User* poUser = GetUser(login.mac_addr);
		if (poUser == NULL)
		{
			poUser = CreateUser(login.mac_addr);
			poUser->SetMacAddr(login.mac_addr);
		}
		NSPROTO::LOGIN_RET loginret;
		if (poUser->GetENetPeer() == NULL)
		{
			event.peer->data = (void*)poUser;
			poUser->SetEnetPeer(event.peer);
			loginret.code = 0;
			XLog(LEVEL_ERROR, "Mac addr %s login successful\n", login.mac_addr);
		}
		else
		{
			loginret.code = 1;
			XLog(LEVEL_ERROR, "Mac addr %s allready login\n", login.mac_addr);
		}
		enet_packet_destroy(event.packet);
		gpoContext->poNetwork->SendMsg(event.peer, 0, ENET_PACKET_FLAG_RELIABLE, &loginret);
		break;
	}
	case NSNETCMD::eBUILD_REQ:
	{
		NSPROTO::BUILD_REQ build = *(NSPROTO::BUILD_REQ*)event.packet;
		User* poUserClient = GetUser(build.mac_client);
		User* poUserServer = GetUser(build.mac_server);

		NSPROTO::BUILD_RET buildret;
		if (poUserClient == NULL || poUserServer == NULL)
		{
			buildret.roomid = 0;
			XLog(LEVEL_ERROR, "Build error client:0x%u server:0x%u\n", (void*)poUserClient, (void*)poUserServer);
		}
		else
		{
			Room* poRoom = CreateRoom();
			poRoom->SetBuildMac(build.mac_client, build.mac_server);
			poUserClient->GetRoomList().push_back(poRoom->GetRoomID());
			poUserServer->GetRoomList().push_back(poRoom->GetRoomID());
			buildret.roomid = poRoom->GetRoomID();
		}
		gpoContext->poNetwork->SendMsg(event.peer, 0, ENET_PACKET_FLAG_RELIABLE, &buildret);
		enet_packet_destroy(event.packet);
		break;
	}
	case NSNETCMD::eUNBUILD_REQ:
	{
		NSPROTO::UNBUILD_REQ unbuild = *(NSPROTO::UNBUILD_REQ*)event.packet;
		Room* poRoom = GetRoom(unbuild.roomid);
		NSPROTO::UNBUILD_RET unbuildret;
		if (poRoom == NULL)
		{
			unbuildret.code = 1;
			XLog(LEVEL_ERROR, "Unbuild room not exist:%d\n", unbuild.roomid);
		}
		else
		{
			unbuildret.code = 0;
			SAFE_DELETE(poRoom);
		}
		gpoContext->poNetwork->SendMsg(event.peer, 0, ENET_PACKET_FLAG_RELIABLE, &unbuildret);
		enet_packet_destroy(event.packet);
		break;
	}
	default:
	{
		break;
	}
	}
}

void RoomMgr::OnDisconnect(ENetEvent& event)
{
	if (event.peer->data == NULL)
	{
		return;
	}
	User* poUser = (User*)event.peer->data;
	poUser->SetEnetPeer(NULL);
	User::RoomList oRoomList = poUser->GetRoomList();
	for (User::RoomIter iter = oRoomList.begin(); iter != oRoomList.end(); iter++)
	{
		Room* poRoom = GetRoom(*iter);
		if (poRoom != NULL)
		{
			SAFE_DELETE(poRoom);
		}
	}
	oRoomList.clear();
}
