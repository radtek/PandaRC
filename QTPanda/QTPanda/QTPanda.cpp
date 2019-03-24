#include "QTPanda.h"
#include "Common/NamedPipe.h"
#include "Common/NetAPI.h"
#include "DllInject.h"
#include <QFile.h>
#include <QTextStream.h>

NamedPipeServer goPipeServer;
LPCSTR lpszProcessName = "MEmuHeadless.exe";
LPCSTR lpszDllName = "E:\\PandaRC\\QTPanda\\Debug\\HACK.dll";

std::string ToHex(uint8_t* data, int len)
{
	char sTmpBuf[8];
	std::string strHex;
	for (int i = 0; i < len; i++)
	{
		sprintf(sTmpBuf, "%02X ", data[i]);
		strHex += sTmpBuf;
	}
	return strHex;
}

void QTPanda::closeEvent(QCloseEvent *event)
{
	unjectDll(lpszProcessName, lpszDllName);
}

void QTPanda::injectDll(LPCSTR lpszProcessName, LPCSTR lpszDllName)
{
	::InjectDll(lpszProcessName, lpszDllName);
}

void QTPanda::unjectDll(LPCSTR lpszProcessName, LPCSTR lpszDllName)
{
	DWORD dwProcessID = FindProcess(lpszProcessName);
	NSPROTO::SRV_EXITTHREAD exit;
	goPipeServer.SendMsg2Client(dwProcessID, &exit);
	::UnjectDll(lpszProcessName, lpszDllName);
}

void QTPanda::WorkderFunc(void* param)
{
	char sIPHost[128] = { 0 };
	char sIPPeer[128] = { 0 };
	char sHeader[1024] = { 0 };
	QTPanda* pParent = (QTPanda*)param;
	MailBox<NSPROTO::COM_DATA>& oMailBox = goPipeServer.GetMailBox();

	QFile ofile("./ouput.txt");
	ofile.remove();
	for (;;)
	{
		NSPROTO::COM_DATA data;
		bool bRet = oMailBox.Recv(&data, 100);
		if (!bRet) continue;
		NetAPI::N2P(data.hostip, sIPHost, sizeof(sIPHost));
		NetAPI::N2P(data.peerip, sIPPeer, sizeof(sIPPeer));
		sprintf(sHeader, "\n%s host=%s:%d peer=%s:%d len=%d data=", data.sparam1, sIPHost, data.hostport, sIPPeer, data.peerport, data.len);
		std::string output = sHeader;
		std::string hex = ToHex((uint8_t*)data.data, data.len);
		output += hex;
		std::string raw(data.data, data.len);
		output += "\nraw=" + raw;
		
		if (ofile.open(QIODevice::Append | QIODevice::WriteOnly))
		{
			QDataStream stream(&ofile);
			stream.writeBytes(output.c_str(), output.size());
			ofile.close();
		}
		//OutputDebugString(output.c_str());
	}
}

QTPanda::QTPanda(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	goPipeServer.Init((HWND)winId());
	m_oWorderThread.Create(&QTPanda::WorkderFunc, this);
} 

void QTPanda::onBtnSendMsg()
{
}

void QTPanda::onBtnInject()
{
	injectDll(lpszProcessName, lpszDllName);
}

void QTPanda::onBtnUnject()
{
	unjectDll(lpszProcessName, lpszDllName);
}
