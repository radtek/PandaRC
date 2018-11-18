#include "Context.h"
#include "Include/Logger/Logger.hpp"

Context* gpoContext = NULL;
int main(int argc, char *argv[])
{
	Logger::Instance()->Init();
	gpoContext = new Context();
	gpoContext->poRoomMgr = new RoomMgr();
	gpoContext->poServer = new Server();
	gpoContext->poServer->Init();
	gpoContext->poServer->Start();

	return 0;
}