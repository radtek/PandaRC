#include "PandaRC.h"
#include <qmath.h>
#include <qpainter.h>
#include <qmessagebox>
#include <QCloseEvent>

#include "Misc.h"
#include "Include/Logger/Logger.hpp"
#include "desktip-ipc/UpdateHandlerServer.h"

PandaRC::PandaRC(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	Logger::Instance()->Init();

	m_pixmap = NULL;
	m_painter = NULL;

	m_netThread = new QNetThread(NULL, this);
	m_netThread->init();
	m_netThread->start();
	m_frameThread = new QFrameThread(NULL ,this);
	m_frameThread->start();
	m_handlerSrv = NULL;
	m_clientViewerRect.setRect(0, 0, 800, 600);

	m_nUserID = 0;

}

void PandaRC::onBtnLogin()
{
	m_netThread->connect("127.0.0.1", 10001, PandaRC::connectedCallback, this);
}

void PandaRC::onBtnBuild()
{
	std::string key = "test";
	ClientViewer* pView = new ClientViewer(this);
	m_clientViewerMap[key] = pView;
	pView->SetKey(key);
	if (m_clientViewerRect.width() != -1)
	{
		pView->setGeometry(m_clientViewerRect);
	}
	else
	{
		pView->showMaximized();
	}
	pView->show();
	
}

void PandaRC::onPaintDataChanged()
{
	do {
		PDEVENT* pd = m_frameThread->getFrame();
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
			int nBytesRow = frame->fb.getBytesPerRow();
			uchar* pBuffer = (uchar*)frame->fb.getBuffer();
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
				//double ratioX = screenSize.width() / bufferSize.width();
				//double ratioY = screenSize.height() / bufferSize.height();
				//SetCursorPos((int)pdcr->pos.x*ratioX, (int)pdcr->pos.y*ratioY);
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

void PandaRC::closeEvent(QCloseEvent *event)
{
	m_netThread->terminate();
	m_frameThread->terminate();
}

void PandaRC::onViewerClose(const std::string& key)
{
	ClientViewer* pViewer = m_clientViewerMap.find(key)->second;
	QPoint pos = pViewer->pos();
	QSize size = pViewer->size();
	m_clientViewerRect.setX(pos.x());
	m_clientViewerRect.setY(pos.y());
	m_clientViewerRect.setWidth(size.width());
	m_clientViewerRect.setHeight(size.height());
	m_clientViewerMap.erase(key);
}

void PandaRC::loginReq()
{
	NSPROTO::LOGIN_REQ login;
	login.userid = 0;
	std::string strMac = NSMisc::getHostMac();
	strcpy(login.mac_addr, strMac.c_str());
	m_netThread->sendMsg(0, ENET_PACKET_FLAG_RELIABLE, &login);
}

void PandaRC::onLoginRet(NSPROTO::PROTO* proto)
{
	NSPROTO::LOGIN_RET loginret = *(NSPROTO::LOGIN_RET*)proto;
	m_nUserID = loginret.userid;
}

void PandaRC::buildReq()
{
	NSPROTO::BUILD_REQ build;
	build.server_userid = 1;
	build.client_userid = 2;
	m_netThread->sendMsg(0, ENET_PACKET_FLAG_RELIABLE, &build);
}

void PandaRC::onBuildRet(int roomID, int service)
{
	m_clientRoomList.push_back(roomID);
	if (service == 1 && m_handlerSrv == NULL)
	{
		m_handlerSrv = new UpdateHandlerServer(this);
	}
}

void PandaRC::onUnbuildRet(int roomID, int service)
{
	if (service == 1)
	{
		for (RoomIter iter = m_clientRoomList.begin(); iter != m_clientRoomList.end(); iter++)
		{
			if (*iter == roomID)
			{
				m_clientRoomList.erase(iter);
				break;
			}
		}
		if (m_clientRoomList.size() <= 0)
		{
			SAFE_DELETE(m_handlerSrv);
		}
	}
}

void PandaRC::connectedCallback(void* param)
{
	PandaRC* pPandaRC = (PandaRC*)param;
	pPandaRC->loginReq();
}

