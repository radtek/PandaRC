#include "Server.h"
#include "Context.h"


Server::Server() 
{
	m_bTerminate = false;
}

Server::~Server()
{
}

bool Server::Init()
{
	m_oNetwork.SetNetInterface(gpoContext->poRoomMgr);
	return m_oNetwork.InitAsServer(10009);
}

void Server::Start()
{
	while (!m_bTerminate)
	{
		m_oNetwork.Update(1);
	}
}
