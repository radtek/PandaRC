#include "QNetThread.h"
#include "Include/Logger/Logger.hpp"
#include "PandaRC.h"
#include "thread/AutoLock.h"

QNetThread::QNetThread(QObject *parent/* = 0*/, QWidget* parentWidget/* = NULL*/)
{
	m_bTerminate = false;
	m_parentWidget = parentWidget;
	m_isConnected = false;
	m_connectedCallback = NULL;
}

bool QNetThread::init()
{
	if (!m_oNetwork.InitAsClient())
	{
		return false;
	}
	m_oNetwork.SetNetInterface(this);
	return true;
}

bool QNetThread::connect(const std::string& strIP, uint16_t uPort, fnConnectedCallback fnConnectedCallback, void* callbackParam)
{
	AutoLock al(&m_oMutex);
	m_connectedCallback = fnConnectedCallback;
	m_callbackParam = callbackParam;
	return m_oNetwork.Connect(strIP, uPort);
}

void QNetThread::run()
{
	while(!m_bTerminate)
	{
		m_updateTimeout.waitForEvent(60);
		AutoLock al(&m_oMutex);
		m_oNetwork.Update(0);
	}
}

bool QNetThread::checkConnected()
{
	if (!m_isConnected)
	{
		XLog(LEVEL_ERROR, "Not connect to server yet!\n");
	}
	return m_isConnected;
}

bool QNetThread::sendMsg(int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto)
{
	AutoLock al(&m_oMutex);
	if (!checkConnected())
	{
		return false;
	}
	return m_oNetwork.Send2Server(nChannel, nPacketFlag, poProto);
}

bool QNetThread::sendMsgRaw(int nChannel, int nPacketFlag, uint8_t* pData, int nSize)
{
	AutoLock al(&m_oMutex);
	if (!checkConnected())
	{
		return false;
	}
	return m_oNetwork.Send2ServerRaw(nChannel, nPacketFlag, pData, nSize);
}

void QNetThread::OnConnect(ENetEvent& event)
{
	XLog(LEVEL_INFO, "Connect success connectid:%u\n", event.peer->connectID);
	m_isConnected = true;
	if (m_connectedCallback != NULL)
	{
		m_connectedCallback(m_callbackParam);
	}
}

void QNetThread::OnReceive(ENetEvent& event)
{
	if (event.packet->dataLength < sizeof(NSPROTO::HEAD))
	{
		XLog(LEVEL_ERROR, "Invalid packet error\n");
		return;
	}

	NSPROTO::HEAD head = *(NSPROTO::HEAD*)event.packet->data;
	if (event.packet->dataLength != head.size)
	{
		XLog(LEVEL_ERROR, "Packet size error cmd:%d size:%d\n", head.cmd, head.size);
		return;
	}

	switch (head.cmd)
	{
		case NSNETCMD::eLOGIN_RET:
		{
			NSPROTO::LOGIN_RET loginret = *(NSPROTO::LOGIN_RET*)event.packet->data;
			if (loginret.code == 0)
			{
				XLog(LEVEL_INFO, "Login successful:%d\n", loginret.userid);
				PandaRC* pPandRC = (PandaRC*)m_parentWidget;
				pPandRC->onLoginRet(&loginret);
			}
			else
			{
				XLog(LEVEL_INFO, "Login fail\n");
			}
			break;
		}
		case NSNETCMD::eBUILD_RET:
		{
			NSPROTO::BUILD_RET buildret = *(NSPROTO::BUILD_RET*)event.packet->data;
			XLog(LEVEL_INFO, "Build room ret roomid:%d\n", buildret.roomid);
			if (buildret.roomid > 0)
			{
				PandaRC* pPandRC = (PandaRC*)m_parentWidget;
				pPandRC->onBuildRet(buildret.roomid, buildret.service);
			}
			break;
		}
		case NSNETCMD::eUNBUILD_RET:
		{
			NSPROTO::UNBUILD_RET unbuildret = *(NSPROTO::UNBUILD_RET*)event.packet->data;
			if (unbuildret.code == 0)
			{
				PandaRC* pPandRC = (PandaRC*)m_parentWidget;
				pPandRC->onUnbuildRet(unbuildret.roomid, unbuildret.service);
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void QNetThread::OnDisconnect(ENetEvent& event)
{
	XLog(LEVEL_INFO, "Disconnect connectid:%d\n", event.peer->connectID);
	m_isConnected = false;
}