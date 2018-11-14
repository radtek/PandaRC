#include "Context.h"

Context* gpoContext = NULL;
int main(int argc, char *argv[])
{
	Context* gpoContext = new Context();
	gpoContext->poRoomMgr = new RoomMgr();

	gpoContext->poNetwork = new Network();
	gpoContext->poNetwork->Init(10004);
	gpoContext->poNetwork->Start();
	return 0;
}