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
	//m_pUDPNet = new UDPNet(parent);
	//m_myThread.start();
	//pHandlerSrv = new UpdateHandlerServer(this);
	//connect(&m_myThread, SIGNAL(paintDataChanged()), this, SLOT(onPaintDataChanged()));
	m_pixmap = NULL;
	m_painter = NULL;

	initENet();
}

bool PandaRC::initENet()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return false;
	}
	m_enetClient = enet_host_create(NULL, 1, 2, 0, 0);
	if (m_enetClient == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
		return false;
	}
	//atexit(enet_deinitialize); //fix pd
	//enet_host_destroy(client); //fix pd
	return true;
}

void PandaRC::onBtnSend()
{
	ENetAddress address;
	ENetEvent event;
	enet_address_set_host_ip(&address, "127.0.0.1");
	address.port = 10001;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	m_enetPeer = enet_host_connect(m_enetClient, &address, 2, 0);
	if (m_enetPeer == NULL)
	{
		fprintf(stderr, "No available peers for initiating an ENet connection.\n");
		return;
	}
	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_enetClient, &event, 3000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		fprintf(stdout, "Connection to 127.0.0.1:10001 succeeded.\n");
	}
	else
	{
		enet_peer_reset(m_enetPeer);
		fprintf(stderr, "Connection to 127.0.0.1:10001 failed.\n");
	}
}

void PandaRC::onBtnRecv()
{
	/* Create a reliable packet of size 7 containing "packet\0" */
	ENetPacket * packet = enet_packet_create("packet", strlen("packet") + 1, ENET_PACKET_FLAG_RELIABLE);
	/* Extend the packet so and append the string "foo", so it now */
	/* contains "packetfoo\0"                                      */
	enet_packet_resize(packet, strlen("packetfoo") + 1);
	strcpy((char*)&packet->data[strlen("packet")], "foo");
	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	enet_peer_send(m_enetPeer, 0, packet);
	/* One could just use enet_host_service() instead. */
	enet_host_flush(m_enetClient);
}

void PandaRC::onPaintDataChanged()
{
	do {
		PDEVENT* pd = m_myThread.getFrame();
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
			if (m_pixmap == NULL)
			{
				m_pixmap = new QPixmap(dim.width, dim.height);
				m_painter = new QPainter(m_pixmap);
			}
			uchar* pBuffer = (uchar*)frame->fb.getBuffer();
			int nBytesRow = frame->fb.getBytesPerRow();
			QImage oImg(pBuffer, frame->rect.getWidth(), frame->rect.getHeight(), nBytesRow, QImage::Format_RGB32);
			m_painter->drawImage(QPoint(frame->rect.left, frame->rect.top), oImg);
			delete pd;

			break;
		}
		case PDEVENT_TYPE::eCURSOR:
		{
			PDCURSOR* pdcr = (PDCURSOR*)pd;

			const QSize& screenSize = size();
			const QSize& bufferSize = m_pixmap->size();
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
	//QPainter painter(this);
	//const QSize& screenSize = size();
	//const QSize& bufferSize = m_pixmap->size();
	//if (bufferSize != screenSize)
	//{
	//	QPixmap scalePixmap = m_pixmap->scaled(screenSize.width(), screenSize.height());
	//	painter.drawPixmap(0, 0, scalePixmap);
	//}
	//else
	//{
	//	painter.eraseRect(0, 0, screenSize.width(), screenSize.height());
	//	painter.drawPixmap(0, 0, *m_pixmap);
	//}

}