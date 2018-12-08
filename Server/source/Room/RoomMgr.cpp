#include "Room/RoomMgr.h"
#include "Context.h"
#include "Common/Network/Network.h"
#include "Include/Logger/Logger.hpp"

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


User* RoomMgr::CreateUser(int nID)
{
	User* poUser = new User(nID);
	m_oUserMap[nID] = poUser;
	return poUser;
}

User* RoomMgr::GetUser(int nID)
{
	UserIter iter = m_oUserMap.find(nID);
	if (iter != m_oUserMap.end())
	{
		return iter->second;
	}
	return NULL;
}

void RoomMgr::RemoveUser(int nID)
{
	UserIter iter = m_oUserMap.find(nID);
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

	NSPROTO::HEAD head = *(NSPROTO::HEAD*)event.packet->data;
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
		NSPROTO::LOGIN_REQ login = *(NSPROTO::LOGIN_REQ*)event.packet->data;
		User* poUser = GetUser(login.userid);
		if (poUser == NULL)
		{
			int nUserID = User::GenID();
			poUser = CreateUser(nUserID);
			poUser->SetMacAddr(login.mac_addr);
		}
		NSPROTO::LOGIN_RET loginret;
		if (poUser->GetENetPeer() == NULL)
		{
			event.peer->data = (void*)poUser;
			poUser->SetEnetPeer(event.peer);
			loginret.code = 0;
			loginret.userid = poUser->GetID();
			XLog(LEVEL_INFO, "User:%d mac addr:%s login successful\n", poUser->GetID(), login.mac_addr);
		}
		else
		{
			loginret.code = 1;
			loginret.userid = poUser->GetID();
			XLog(LEVEL_ERROR, "User:%d mac addr:%s allready login\n", poUser->GetID(), login.mac_addr);
		}
		enet_packet_destroy(event.packet);
		gpoContext->poServer->GetNetwork()->Send2Client(event.peer, 0, ENET_PACKET_FLAG_RELIABLE, &loginret);
		break;
	}
	case NSNETCMD::eBUILD_REQ:
	{
		NSPROTO::BUILD_REQ build = *(NSPROTO::BUILD_REQ*)event.packet->data;
		User* poUserClient = GetUser(build.client_userid);
		User* poUserServer = GetUser(build.server_userid);

		NSPROTO::BUILD_RET buildret;
		if (poUserClient == NULL || poUserServer == NULL)
		{
			buildret.roomid = 0;
			XLog(LEVEL_ERROR, "Build error client:0x%x server:0x%x\n", (void*)poUserClient, (void*)poUserServer);
		}
		else
		{
			Room* poRoom = CreateRoom();
			poRoom->SetBuildUser(build.client_userid, build.server_userid);
			poUserServer->GetClientRoomList().push_back(poRoom->GetRoomID());
			buildret.roomid = poRoom->GetRoomID();
		}
		buildret.service = 1;
		gpoContext->poServer->GetNetwork()->Send2Client(poUserServer->GetENetPeer(), 0, ENET_PACKET_FLAG_RELIABLE, &buildret);
		buildret.service = 2;
		gpoContext->poServer->GetNetwork()->Send2Client(poUserClient->GetENetPeer(), 0, ENET_PACKET_FLAG_RELIABLE, &buildret);
		enet_packet_destroy(event.packet);
		break;
	}
	case NSNETCMD::eUNBUILD_REQ:
	{
		NSPROTO::UNBUILD_REQ unbuild = *(NSPROTO::UNBUILD_REQ*)event.packet->data;
		Room* poRoom = GetRoom(unbuild.roomid);
		NSPROTO::UNBUILD_RET unbuildret;
		if (poRoom == NULL)
		{
			unbuildret.code = 1;
			unbuild.roomid = unbuild.roomid;
			gpoContext->poServer->GetNetwork()->Send2Client(event.peer, 0, ENET_PACKET_FLAG_RELIABLE, &unbuildret);
			XLog(LEVEL_ERROR, "Unbuild room not exist:%d\n", unbuild.roomid);
		}
		else
		{
			unbuildret.code = 0;
			unbuild.roomid = unbuild.roomid;
			User* pClientUser = GetUser(poRoom->GetClientUserID());
			User* pServerUser = GetUser(poRoom->GetServerUserID());
			pServerUser->RemoveClientRoomID(unbuild.roomid);
			unbuildret.service = 1;
			gpoContext->poServer->GetNetwork()->Send2Client(pServerUser->GetENetPeer(), 0, ENET_PACKET_FLAG_RELIABLE, &unbuildret);
			unbuildret.service = 2;
			gpoContext->poServer->GetNetwork()->Send2Client(pClientUser->GetENetPeer(), 0, ENET_PACKET_FLAG_RELIABLE, &unbuildret);
			SAFE_DELETE(poRoom);
		}
		enet_packet_destroy(event.packet);
		break;
	}
	case NSNETCMD::eFRAME_SYNC:
	{
		NSPROTO::FRAME_SYNC* frame = (NSPROTO::FRAME_SYNC*)event.packet->data;
		User* poServer = GetUser(frame->srv_uid);
		if (poServer == NULL)
		{
			XLog(LEVEL_ERROR, "Server user not exist:%d\n", frame->srv_uid);
		}
		else
		{
			User::RoomList& oRoomList = poServer->GetClientRoomList();
			for (User::RoomIter iter = oRoomList.begin(); iter != oRoomList.end(); iter++)
			{
				Room* poRoom = GetRoom(*iter);
				if (poRoom != NULL)
				{
					poRoom->SendToClient(event.packet->data, event.packet->dataLength, 1);
				}
			}
		}
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
	User::RoomList oRoomList = poUser->GetClientRoomList();
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
