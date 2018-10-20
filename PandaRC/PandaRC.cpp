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

	m_nPixmap = new QPixmap(1366,768);
	m_nPainter = new QPainter(m_nPixmap);
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
		PDFRAME* pd = m_oMyThread.getFrame();
		if (pd == NULL)
			break;

		Dimension dim = pd->fb.getDimension();
		uchar* pBuffer = (uchar*)pd->fb.getBuffer();
		int nBytesRow = pd->fb.getBytesPerRow();
		QImage oImg(pBuffer, pd->rect.getWidth(), pd->rect.getHeight(), nBytesRow, QImage::Format_RGB32);
		m_nPainter->drawImage(QPoint(pd->rect.left, pd->rect.top), oImg);
		delete pd;
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
	const QSize& bufferSize = m_nPixmap->size();
	if (bufferSize != screenSize)
	{
		QPixmap scalePixmap = m_nPixmap->scaled(screenSize.width(), screenSize.height());
		painter.drawPixmap(0, 0, scalePixmap);
	}
	else
	{
		painter.eraseRect(0, 0, screenSize.width(), screenSize.height());
		painter.drawPixmap(0, 0, *m_nPixmap);
	}

}