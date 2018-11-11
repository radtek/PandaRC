#pragma once
#include "Common/Platform.h"
#include "enet/enet.h"

class User
{
public:
	User();
	virtual ~User() {};

	std::string& GetMacAddr() { return m_oMacAddr; }
	void SetMacAddr(const std::string& oMacAddr) { m_oMacAddr = oMacAddr; }

	ENetPeer* GetENetPeer() { return m_poENetPeer; }
	void SetEnetPeer(ENetPeer* poENetPeer) { m_poENetPeer = poENetPeer; }

private:
	ENetPeer* m_poENetPeer;
	std::string m_oMacAddr;
};
