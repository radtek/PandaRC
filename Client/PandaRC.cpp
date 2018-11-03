#include "PandaRC.h"
#include <qpainter.h>
#include <qmath.h>

#include "log-writer/LogWriter.h"
#include "desktip-ipc/UpdateHandlerServer.h"

LogWriter glog;
UpdateHandlerServer* pHandlerSrv;


PandaRC::PandaRC(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//Logger::Instance()->Init();

	m_oMyThread.start();
	m_pUDPNet = new UDPNet(parent);
	pHandlerSrv = new UpdateHandlerServer(this);
	connect(&m_oMyThread, SIGNAL(paintDataChanged()), this, SLOT(onPaintDataChanged()));

	m_pPixmap = NULL;
	m_pPainter = NULL;
}

void PandaRC::onBtnSend()
{
}

void PandaRC::onBtnRecv()
{
}

void PandaRC::onPaintDataChanged()
{
	do {
		PDEVENT* pd = m_oMyThread.getFrame();
		if (pd == NULL)
		{
			break;
		}
		switch (pd->type)
		{
		case PDEVENT_TYPE::eFRAME:
		{
			PDFRAME* frame = (PDFRAME*)pd;
			Dimension dim = frame->fb.getDimension();
			if (m_pPixmap == NULL)
			{
				m_pPixmap = new QPixmap(dim.width, dim.height);
				m_pPainter = new QPainter(m_pPixmap);
			}
			uchar* pBuffer = (uchar*)frame->fb.getBuffer();
			int nBytesRow = frame->fb.getBytesPerRow();
			QImage oImg(pBuffer, frame->rect.getWidth(), frame->rect.getHeight(), nBytesRow, QImage::Format_RGB32);
			m_pPainter->drawImage(QPoint(frame->rect.left, frame->rect.top), oImg);
			delete pd;

			break;
		}
		case PDEVENT_TYPE::eCURSOR:
		{
			PDCURSOR* pdcr = (PDCURSOR*)pd;

			const QSize& screenSize = size();
			const QSize& bufferSize = m_pPixmap->size();
			//if (pdcr->shapeChange)
			//{
			//	QPixmap pixmap(pdcr->dim.width, pdcr->dim.height);
			//	pixmap.loadFromData((const uchar*)pdcr->buffer, pdcr->bufferSize);
			//	QCursor cursor(pixmap, pdcr->hotspot.x, pdcr->hotspot.y);
			//	if (pdcr->posChange)
			//	{
			//		cursor.setPos(pdcr->pos.x, pdcr->pos.y);
			//	}
			//	else
			//	{
			//		const QCursor& orgCursor = QWidget::cursor();
			//		cursor.setPos(orgCursor.pos().x(), orgCursor.pos().y());
			//	}
			//	setCursor(cursor);
			//}
			if (pdcr->posChange)// && !pdcr->shapeChange)
			{
				double ratioX = screenSize.width() / bufferSize.width();
				double ratioY = screenSize.height() / bufferSize.height();
				SetCursorPos((int)pdcr->pos.x*ratioX, (int)pdcr->pos.y*ratioY);
			}
			delete pd;

			break;
		}
		}

	} while (true);

	update();
}

void PandaRC::paintEvent(QPaintEvent *event)
{
	//if (m_pd == NULL)
	//	return;
	//PDFRAME* pd = m_pd;
	//Dimension dim = pd->fb.getDimension();

	//uchar* pBuffer = (uchar*)pd->fb.getBuffer();
	//int nBytesRow = pd->fb.getBytesPerRow();
	//QImage oImg(pBuffer, dim.width, dim.height, nBytesRow, QImage::Format_RGB32);

	//QPainter painter(this);
	//painter.drawImage(QPoint(pd->rect.left, pd->rect.top), oImg);
	//m_pd = NULL;
	//delete pd;

	QPainter painter(this);
	const QSize& screenSize = size();
	const QSize& bufferSize = m_pPixmap->size();
	if (bufferSize != screenSize)
	{
		QPixmap scalePixmap = m_pPixmap->scaled(screenSize.width(), screenSize.height());
		painter.drawPixmap(0, 0, scalePixmap);
	}
	else
	{
		painter.eraseRect(0, 0, screenSize.width(), screenSize.height());
		painter.drawPixmap(0, 0, *m_pPixmap);
	}

}