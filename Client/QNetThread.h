#pragma once

#include <queue>
#include <QThread>
#include "Common/Network/Network.h"
#include "win-system/WindowsEvent.h"
#include "thread/LocalMutex.h"

class QNetThread : public QThread
{
	Q_OBJECT
public:
	explicit QNetThread(QObject *parent = 0);
	virtual void run();

public:
	bool init();
	void terminate() { m_bTerminate = true; }
	bool connect(const std::string& strIP, uint16_t uPort);
	bool sendMsg(int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto);
	bool sendMsgRaw(int nChannel, int nPacketFlag, uint8_t* pData, int nSize);

protected:
	bool m_bTerminate;
	Network m_oNetwork;
	LocalMutex m_oMutex;
	WindowsEvent m_updateTimeout;
};
