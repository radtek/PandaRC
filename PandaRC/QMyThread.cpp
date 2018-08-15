#include "QMyThread.h"
#include "thread/AutoLock.h"

QMyThread::QMyThread(QObject *parent) :QThread(parent)
{

}

void QMyThread::run()
{
	for (;;)
	{
		m_updateTimeout.waitForEvent(30);
		m_pdMutex.lock();
		while (m_pdList.size() > 0)
		{
			PDFRAME* pd = m_pdList.front();
			m_pdList.pop();
			emit paintDataChanged(pd);
		}
		m_pdMutex.unlock();
	}
}

void QMyThread::addFrame(PDFRAME* pd)
{
	AutoLock al(&m_pdMutex);
	m_pdList.push(pd);
}