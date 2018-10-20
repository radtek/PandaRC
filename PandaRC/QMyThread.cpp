#include "QMyThread.h"
#include "thread/AutoLock.h"

QMyThread::QMyThread(QObject *parent) :QThread(parent)
{

}

void QMyThread::run()
{
	for (;;)
	{
		m_updateTimeout.waitForEvent(60);
		AutoLock al(&m_pdMutex);
		if (m_oneFrame.size() > 0)
		{
			continue;
		}
		while (m_pdList.size() > 0)
		{
			PDFRAME* frame = m_pdList.front();
			m_pdList.pop();
			m_oneFrame.push(frame);
		}
		if (m_oneFrame.size() > 0)
		{
			emit paintDataChanged();
		}
	}
}

void QMyThread::addFrame(PDFRAME* pd)
{
	AutoLock al(&m_pdMutex);
	m_pdList.push(pd);
}

PDFRAME* QMyThread::getFrame()
{
	AutoLock al(&m_pdMutex);
	if (m_oneFrame.size() <= 0)
		return NULL;
	PDFRAME* pd  = m_oneFrame.front();
	m_oneFrame.pop();
	return pd;
}

PDFRAME* QMyThread::peekFrame()
{
	AutoLock al(&m_pdMutex);
	if (m_oneFrame.size() <= 0)
		return NULL;
	PDFRAME* pd  = m_oneFrame.front();
	return pd;
}
