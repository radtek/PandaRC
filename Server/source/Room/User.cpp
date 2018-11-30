#include "Room/User.h"

User::User(int nID)
{
	m_nID = nID;
	m_poENetPeer = NULL;
}

void User::RemoveClientRoomID(int nRoomID)
{
	RoomIter iter = m_oClientRoomList.begin();
	for (; iter != m_oClientRoomList.end(); iter++)
	{
		if (*iter == nRoomID)
		{
			m_oClientRoomList.erase(iter);
			break;
		}
	}
}

int User::GenID()
{
	static int nAutoID = 0;
	nAutoID = nAutoID % 0x7FFFFFFF + 1;
	return nAutoID;
}
