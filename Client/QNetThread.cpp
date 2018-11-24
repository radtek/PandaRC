#include "QNetThread.h"
#include "thread/AutoLock.h"

QNetThread :: QNetThread(QObject *parent/* = 0*/)
{
	m_bTerminate = false;
}

bool QNetThread::init()
{
	return m_oNetwork.InitAsClient();
}

bool QNetThread::connect(const std::string& strIP, uint16_t uPort)
{
	AutoLock al(&m_oMutex);
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

bool QNetThread::sendMsg(int nChannel, int nPacketFlag, NSPROTO::PROTO* poProto)
{
	AutoLock al(&m_oMutex);
	return m_oNetwork.Send2Server(nChannel, nPacketFlag, poProto);
}

bool QNetThread::sendMsgRaw(int nChannel, int nPacketFlag, uint8_t* pData, int nSize)
{
	AutoLock al(&m_oMutex);
	return m_oNetwork.Send2ServerRaw(nChannel, nPacketFlag, pData, nSize);
}
