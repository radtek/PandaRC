#include "QTPanda.h"
#include "Common/NamedPipe.h"
#include "Common/NetAPI.h"
#include "Decoder/websocket.h"
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
	m_bShutDown = true;
	m_oWorderThread.Join();
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
	while (!pParent->m_bShutDown)
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
	m_bShutDown = false;
	goPipeServer.Init((HWND)winId());
	m_oWorderThread.Create(&QTPanda::WorkderFunc, this);
} 

void QTPanda::onBtnSendMsg()
{
	uint8_t data[] = {0x93, 0x48, 0x14, 0xB3, 0x14, 0x17, 0x23, 0x8A, 0xD9, 0xBA, 0x3F, 0x51, 0x12, 0x00, 0x00, 0x00, 0x41, 0xFF, 0xD8, 0x8D, 0x50, 0xBB, 0x74, 0xFE, 0xE5, 0xB3, 0x1A, 0x4A, 0x30, 0x06, 0x4F, 0x2E, 0x1A, 0x42, 0x8C, 0xFF, 0x94, 0x22, 0x68, 0x6F, 0x35, 0xFA, 0x31, 0xDF, 0xA9, 0xC9, 0x1B, 0x71, 0x41, 0x12, 0xAF, 0xDD, 0x10, 0xAC, 0xDF, 0x63, 0xD7, 0xE7, 0x25, 0x01, 0x86, 0xF8, 0x86, 0x64, 0xE3, 0x57, 0x6A, 0x51, 0xD2, 0x48, 0x3C, 0xA8, 0x47, 0xBF, 0xB3, 0xA5, 0x90, 0xC7, 0x93, 0x48, 0x14, 0xB3, 0x14, 0x81, 0x22, 0x8A, 0xD9, 0x17, 0xC7, 0xA9, 0x12, 0x00, 0x00, 0x00, 0x97, 0xAA, 0x8A, 0x53, 0xA3, 0x3C, 0xA1, 0x46, 0xF3, 0x5C, 0x84, 0x11, 0x9E, 0x76, 0xD8, 0x63, 0xB7, 0xAC, 0xE7, 0x1C, 0x00, 0x3D, 0x22, 0xB2, 0x02, 0x0A, 0xAD, 0x53, 0x47, 0x72, 0xF7, 0xC1, 0x90, 0x0B, 0xEE, 0x0C, 0xF5, 0x87, 0x3B, 0xE9, 0x36, 0xCF, 0x3E, 0xDA, 0x77, 0x23, 0x45, 0x11, 0x9D, 0x92, 0x4F, 0xED, 0x79, 0xD4, 0x58, 0x4E, 0x89, 0xDC, 0xC9, 0xB9, 0x01, 0x63};
	uint8_t decode[sizeof(data)] = { 0 };
	int dlen = 0;
	NSWSD::DecodePacket(data, sizeof(data), 0, decode, dlen);
	OutputDebugString((char*)data);
	OutputDebugString("\n");
	OutputDebugString((char*)decode);
}

void QTPanda::onBtnInject()
{
	injectDll(lpszProcessName, lpszDllName);
}

void QTPanda::onBtnUnject()
{
	unjectDll(lpszProcessName, lpszDllName);
}
