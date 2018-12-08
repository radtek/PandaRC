#include "Room/Room.h"
#include "Context.h"

Room::Room(int nRoomID)
{
	m_nRoomID = nRoomID;
}

Room::~Room()
{
	User* poServerUser = gpoContext->poRoomMgr->GetUser(m_nServerUserID);
	if (poServerUser != NULL)
	{
		poServerUser->RemoveClientRoomID(m_nRoomID);
	}
}

void Room::SetBuildUser(int nClientUserID, int nServerUserID)
{
	m_nClientUserID = nClientUserID;
	m_nServerUserID = nServerUserID;
}

void Room::SendToClient(uint8_t* pData, int nLen, int nChannel)
{
	User* poClientUser = gpoContext->poRoomMgr->GetUser(m_nClientUserID);
	if (poClientUser != NULL)
	{
		gpoContext->poServer->GetNetwork()->Send2ClientRaw(poClientUser->GetENetPeer(), nChannel, ENET_PACKET_FLAG_RELIABLE, pData, nLen);
	}
}
