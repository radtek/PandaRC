#pragma once
#include "Common/Platform.h"

class Room
{
public:
	Room(int nRoomID);
	virtual ~Room();
	int GetRoomID() { return m_nRoomID; }

	void SetBuildUser(int nClientUserID, int nServerUserID);
	int GetClientUserID() { return m_nClientUserID; }
	int GetServerUserID() { return m_nServerUserID; }
	void SendToClient(uint8_t* pData, int nLen, int nChannel);

private:
	int m_nRoomID;
	int m_nClientUserID;
	int m_nServerUserID;
};