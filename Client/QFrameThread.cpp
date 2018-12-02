#include "Common/DataStruct/XMath.h"

#include "Include/Logger/Logger.hpp"
#include "PandaRC.h"
#include "QFrameThread.h"
#include "thread/AutoLock.h"

QFrameThread::QFrameThread(QObject *parent, QWidget* parentWidget):QThread(parent)
{
	m_pixmap = NULL;
	m_painter = NULL;
	m_parentWidget = parentWidget;
}

QFrameThread::~QFrameThread()
{
	SAFE_DELETE(m_painter);
	SAFE_DELETE(m_pixmap);
}

void QFrameThread::run()
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
			PDEVENT* frame = m_pdList.front();
			m_pdList.pop();
			m_oneFrame.push(frame);
		}

		bool displayChange = false;
		QRect maxRect(0, 0, 0, 0);
		while (m_oneFrame.size() > 0)
		{
			PDEVENT* pd = m_oneFrame.front();
			m_oneFrame.pop();

			if (pd->type == PDEVENT_TYPE::eFRAME)
			{
				displayChange = true;
				PDFRAME* frame = (PDFRAME*)pd;
				QImage oImg((uchar*)frame->fb.getBuffer(), frame->rect.getWidth(), frame->rect.getHeight(), frame->fb.getBytesPerRow(), QImage::Format_RGB16);
				m_painter->drawImage(QPoint(frame->rect.left, frame->rect.top), oImg);

				maxRect.setLeft(XMath::Min((int)maxRect.left(), (int)frame->rect.left));
				maxRect.setTop(XMath::Min((int)maxRect.top(), (int)frame->rect.top));
				maxRect.setWidth(XMath::Max((int)maxRect.width(), (int)frame->rect.getWidth()));
				maxRect.setHeight(XMath::Max((int)maxRect.height(), (int)frame->rect.getHeight()));
			}
			delete pd;
		}

		if (displayChange)
		{
			QImage fullImg = m_pixmap->toImage();
			QImage copyImg = fullImg.copy(maxRect);
			uchar* imgData = copyImg.bits();
			qsizetype imgSize = copyImg.sizeInBytes();

			PandaRC* pandaRC = ((PandaRC*)m_parentWidget);

			NSPROTO::FRAME_SYNC sync;
			sync.userid = pandaRC->getUserID();
			sync.left = maxRect.left();
			sync.top = maxRect.top();
			sync.width = maxRect.width();
			sync.height = maxRect.height();
			sync.setData(imgData, imgSize);

			int nSize = 0;
			uint8_t* pData = NULL;
			sync.getData(&pData, nSize);
			pandaRC->getNetThread()->sendMsgRaw(1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT, pData, nSize);
		}
	}
}

void QFrameThread::addFrame(PDEVENT* pd)
{
	AutoLock al(&m_pdMutex);
	m_pdList.push(pd);

	if (pd->type == PDEVENT_TYPE::eFRAME && m_pixmap == NULL)
	{
		PDFRAME* frame = (PDFRAME*)pd;
		Dimension dim = frame->fb.getDimension();
		m_pixmap = new QPixmap(dim.width, dim.height);
		m_painter = new QPainter(m_pixmap);
	}
}

PDEVENT* QFrameThread::getFrame()
{
	AutoLock al(&m_pdMutex);
	if (m_oneFrame.size() <= 0)
	{
		return NULL;
	}
	PDEVENT* pd  = m_oneFrame.front();
	m_oneFrame.pop();
	return pd;
}

PDEVENT* QFrameThread::peekFrame()
{
	AutoLock al(&m_pdMutex);
	if (m_oneFrame.size() <= 0)
	{
		return NULL;
	}
	PDEVENT* pd  = m_oneFrame.front();
	return pd;
}
