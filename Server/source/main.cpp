#include "Context.h"
#include "Include/Logger/Logger.hpp"

Context* gpoContext = NULL;
int main(int argc, char *argv[])
{
	Logger::Instance()->Init();
	Context* gpoContext = new Context();
	gpoContext->poRoomMgr = new RoomMgr();

	gpoContext->poNetwork = new Network();
	gpoContext->poNetwork->Init();
	gpoContext->poNetwork->Listen(10001);
	gpoContext->poNetwork->Start();
	return 0;
}