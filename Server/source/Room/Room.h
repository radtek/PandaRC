#pragma once
#include "Common/Platform.h"

class Room
{
public:
	Room(int nRoomID);
	virtual ~Room();
	int GetRoomID() { return m_nRoomID; }
	void SetBuildMac(const std::string& oClientMac, const std::string& oServerMac);

private:
	int m_nRoomID;
	std::string m_oClientMac;
	std::string m_oServerMac;
};