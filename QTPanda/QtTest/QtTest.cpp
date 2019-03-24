#include "QtTest.h"
#include "Common/NamedPipe.h"

NamedPipeServer oServer;

QtTest::QtTest(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	oServer.Init((HWND)winId());

	//HWND hForm = FindWindow(NULL, "QTPanda");
	//COPYDATASTRUCT cds;
	//cds.dwData = sizeof(COPYDATASTRUCT);
	//cds.cbData = (DWORD)strlen("test test test******");
	//cds.lpData = "test test test******";
	//SendMessage(hForm, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
}
