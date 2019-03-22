#include "QTPanda.h"
#include "DllInject.h"
#include "Common/NamedPipe.h"

NamedPipeClient oClient;

QTPanda::QTPanda(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//sprintf(szOutputBuf, "mai form:%u", (HWND)winId());
	//MessageBox(NULL, szOutputBuf, "пео╒", MB_ICONINFORMATION);
	oClient.Init();
} 

bool QTPanda::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	MSG* msg = (MSG*)message;
	if (msg->message == WM_COPYDATA)
	{
		COPYDATASTRUCT* p = reinterpret_cast<COPYDATASTRUCT*>(msg->lParam);
		OutputDebugStringA(static_cast<char*>(p->lpData));
	}
	return QMainWindow::nativeEvent(eventType, message, result);
}

void QTPanda::onBtnSendMsg()
{
	NSPROTO::DATA data;
	data.un.iparam1 = 1;
	oClient.SendMsg2Server(&data);
	//HWND hwnd = (HWND)winId();
	//char buf[32] = { 0 };
	//sprintf(buf, "%u\n", hwnd);
	//OutputDebugStringA(buf);

	//COPYDATASTRUCT cds;
	//cds.dwData = sizeof(COPYDATASTRUCT);
	//cds.cbData = (DWORD)strlen("hehehe");
	//cds.lpData = "hehehe";
	//SendMessage(hwnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
}
void QTPanda::onBtnInject()
{
	InjectDll("xshellcore.exe", "F:\\Test\\QTPanda\\Debug\\HACK.dll");
}

void QTPanda::onBtnUnject()
{
	UnjectDll("xshellcore.exe", "HACK.dll");
}
