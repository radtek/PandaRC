#pragma once
#include "Common/Network/Network.h"

class Server
{
public:
	Server();
	virtual ~Server();

	bool Init();
	void Start();
	void Terminate() { m_bTerminate = true; }
	Network* GetNetwork() { return &m_oNetwork; }

private:
	bool m_bTerminate;
	Network m_oNetwork;
};
