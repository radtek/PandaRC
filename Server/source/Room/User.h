#pragma once
#include "Common/Platform.h"

class User
{
public:
	User() {};
	virtual ~User() {};

	std::string& GetMacAddr() { return m_oMacAddr; }
	void SetMacAddr(const std::string& oMacAddr) { m_oMacAddr = oMacAddr; }

private:
	std::string m_oMacAddr;
};
