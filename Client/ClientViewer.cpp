#include "ClientViewer.h"
#include "PandaRC.h"

ClientViewer::ClientViewer(QWidget *parent)
	: QWidget(parent, Qt::Window)
{
	ui.setupUi(this);
}

ClientViewer::~ClientViewer()
{
}

void ClientViewer::closeEvent(QCloseEvent *event)
{
	PandaRC* pPandaRC = (PandaRC*)parentWidget();
	pPandaRC->onViewerClose(m_userID);
}

void ClientViewer::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	const QSize& screenSize = size();
	const QSize& bufferSize = m_pixmap->size();
	if (bufferSize != screenSize)
	{
		QPixmap scalePixmap = m_pixmap->scaled(screenSize.width(), screenSize.height());
		painter.drawPixmap(0, 0, scalePixmap);
	}
	else
	{
		painter.eraseRect(0, 0, screenSize.width(), screenSize.height());
		painter.drawPixmap(0, 0, *m_pixmap);
	}
}
