#include "Common/DataStruct/XMath.h"

#include "Include/Logger/Logger.hpp"
#include "QFrameThread.h"
#include "thread/AutoLock.h"
#include "PandaRC.h"
#include "Misc.h"

QFrameThread::QFrameThread(QObject *parent, QWidget* parentWidget):QThread(parent)
{
	m_parentWidget = parentWidget;
	m_pixmap = NULL;
	m_painter = NULL;
}

QFrameThread::~QFrameThread()
{
	SAFE_DELETE(m_pixmap);
	SAFE_DELETE(m_painter);
}

void QFrameThread::run()
{
	int frame = 1000 / 5;
	for (;;)
	{
		m_updateTimeout.waitForEvent(frame);
		AutoLock al(&m_pdMutex);
		updateServer();
		updateClient();
	}
}

void QFrameThread::combindFrame(NSPROTO::FRAME_SYNC& sync, PDEVENT* pd)
{
	PandaRC* pandaRC = ((PandaRC*)m_parentWidget);
	if (pd->type == PDEVENT_TYPE::eFRAME)
	{
		PDFRAME* fm = (PDFRAME*)pd;
		sync.srv_uid = pandaRC->getUserID();

		int compressSize = 0;
		uint8_t* compressBuffer = NULL;
		bool res = NSMisc::zipCompress((uint8_t*)fm->fb.getBuffer(), fm->fb.getBufferSize(), &compressBuffer, compressSize);
		if (res)
		{
			sync.setBuffer(fm->rect.left, fm->rect.top, fm->rect.getWidth(), fm->rect.getHeight(), fm->fb.getPixelFormat().bitsPerPixel, compressBuffer, compressSize);
		}
		else
		{
			XLog(LEVEL_INFO, "compress fail %d->%d\n", res, fm->fb.getBufferSize(), compressSize);
		}
	}
}

void QFrameThread::updateServer()
{
	PandaRC* pandaRC = ((PandaRC*)m_parentWidget);

	NSPROTO::FRAME_SYNC sync;
	while (m_pdServerList.size() > 0)
	{
		PDEVENT* pd = m_pdServerList.front();
		m_pdServerList.pop();
		combindFrame(sync, pd);
		delete pd;
	}

	if (sync.frameNum > 0)
	{
		int size = 0;
		uint8_t* data = NULL;
		sync.getData(&data, size);
		pandaRC->getNetThread()->sendMsgRaw(1, ENET_PACKET_FLAG_RELIABLE, data, size);;
	}
}

void QFrameThread::updateClient()
{
	if (m_oneClientFrame.size() > 0)
	{
		return;
	}
	while (m_pdClientList.size() > 0)
	{
		PDEVENT* pd = m_pdClientList.front();
		m_pdClientList.pop();
		m_oneClientFrame.push(pd);
	}

	if (m_oneClientFrame.size() > 0)
	{
		emit paintDataChanged();
	}
}

void QFrameThread::addServerFrame(PDEVENT* pd)
{
	AutoLock al(&m_pdMutex);
	m_pdServerList.push(pd);
}

void QFrameThread::addClientFrame(PDEVENT* pd)
{
	AutoLock al(&m_pdMutex);
	m_pdClientList.push(pd);
}

PDEVENT* QFrameThread::getClientFrame()
{
	AutoLock al(&m_pdMutex);
	if (m_oneClientFrame.size() <= 0)
	{
		return NULL;
	}
	PDEVENT* pd  = m_oneClientFrame.front();
	m_oneClientFrame.pop();
	return pd;
}
