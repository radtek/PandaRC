#include "Common/DataStruct/XMath.h"

#include "Include/Logger/Logger.hpp"
#include "PandaRC.h"
#include "QFrameThread.h"
#include "thread/AutoLock.h"

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
	int frame = 1000 / 10;
	for (;;)
	{
		m_updateTimeout.waitForEvent(frame);
		AutoLock al(&m_pdMutex);
		updateServer();
		updateClient();
	}
}

void QFrameThread::updateServer()
{
	PandaRC* pandaRC = ((PandaRC*)m_parentWidget);

	//bool displayChange = false;
	//QRect validRect(0, 0, 0, 0);
	while (m_pdServerList.size() > 0)
	{
		PDEVENT* pd = m_pdServerList.front();
		m_pdServerList.pop();

		if (pd->type == PDEVENT_TYPE::eFRAME)
		{
			PDFRAME* fm = (PDFRAME*)pd;

			NSPROTO::FRAME_SYNC sync;
			sync.srv_uid = pandaRC->getUserID();
			sync.left = fm->rect.left;
			sync.top = fm->rect.top;
			sync.width = fm->rect.getWidth();
			sync.height = fm->rect.getHeight();
			sync.bitsPerPixel = fm->fb.getPixelFormat().bitsPerPixel;
			sync.setBuffer((uint8_t*)fm->fb.getBuffer(), fm->fb.getBufferSize());

			int nSize = 0;
			uint8_t* pData = NULL;
			sync.getData(&pData, nSize);
			pandaRC->getNetThread()->sendMsgRaw(1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT, pData, nSize);;
		}
		delete pd;

		//if (pd->type == PDEVENT_TYPE::eFRAME)
		//{
		//	displayChange = true;
		//	PDFRAME* frame = (PDFRAME*)pd;
		//	if (m_pixmap == NULL)
		//	{
		//		m_pixmap = new QPixmap(frame->rect.getWidth(), frame->rect.getHeight());
		//		m_painter = new QPainter(m_pixmap);
		//	}
		//	QImage img((uchar*)frame->fb.getBuffer(), frame->rect.getWidth(), frame->rect.getHeight(), frame->fb.getBytesPerRow(), QImage::Format_RGB32);
		//	m_painter->drawImage(QPoint(frame->rect.left, frame->rect.top), img);

		//	QRect tmpRect = QRect(frame->rect.left, frame->rect.top, frame->rect.getWidth(), frame->rect.getHeight());
		//	validRect = validRect.united(tmpRect);
		//}
		//delete pd;
	}

	//if (displayChange)
	//{
	//	QImage fullImg = m_pixmap->toImage();
	//	QImage copyImg = fullImg.copy(validRect);
	//	const uchar* imgData = copyImg.constBits();
	//	qsizetype imgSize = copyImg.sizeInBytes();

	//	NSPROTO::FRAME_SYNC sync;
	//	sync.srv_uid = pandaRC->getUserID();
	//	sync.left = validRect.left();
	//	sync.top = validRect.top();
	//	sync.width = validRect.width();
	//	sync.height = validRect.height();
	//	sync.bitsPerPixel = -1;
	//	sync.setBuffer(imgData, imgSize);

	//	int nSize = 0;
	//	uint8_t* pData = NULL;
	//	sync.getData(&pData, nSize);
	//	pandaRC->getNetThread()->sendMsgRaw(1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT, pData, nSize);
	//}

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
