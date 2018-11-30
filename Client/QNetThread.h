#pragma once

#include <queue>
#include <QThread>
#include "Common/Network/Network.h"
#include "win-system/WindowsEvent.h"
#include "thread/LocalMutex.h"

typedef void(*fnConnectedCallback)(void* param);
class QNetThread : public QThread, public NetInterface
{
	Q_OBJECT
public:
	explicit QNetThread(QObject *parent = 0, QWidget* parentWidget = NULL);
	virtual void run();

public:
	bool init();
	void terminate() { m_bTerminate = true; }
	bool connect(const std::string& strIP, uint16_t uPort, fnConnectedCallback fnConnectedCallback = NULL, void* callbackParam = NULL);
	bool sendMsg(int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto);
	bool sendMsgRaw(int nChannel, int nPacketFlag, uint8_t* pData, int nSize);
	bool isConnected() { return m_isConnected; }

protected:
	virtual void OnConnect(ENetEvent& event);
	virtual void OnReceive(ENetEvent& event);
	virtual void OnDisconnect(ENetEvent& event);
	bool checkConnected();

protected:
	bool m_bTerminate;
	Network m_oNetwork;
	LocalMutex m_oMutex;
	WindowsEvent m_updateTimeout;
	QWidget* m_parentWidget;
	bool m_isConnected;
	fnConnectedCallback m_connectedCallback;
	void* m_callbackParam;
};
