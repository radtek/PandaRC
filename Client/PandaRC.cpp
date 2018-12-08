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
	ClientViewer* pView = new ClientViewer(this);
	m_clientViewerMap[1] = pView;
	pView->setUserID(1);
	if (m_clientViewerRect.width() != -1)
	{
		pView->setGeometry(m_clientViewerRect);
	}
	else
	{
		pView->showMaximized();
	}
	pView->show();
	buildReq();
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

void PandaRC::onViewerClose(int userID)
{
	ClientViewer* pViewer = m_clientViewerMap.find(userID)->second;
	QPoint pos = pViewer->pos();
	QSize size = pViewer->size();
	m_clientViewerRect.setX(pos.x());
	m_clientViewerRect.setY(pos.y());
	m_clientViewerRect.setWidth(size.width());
	m_clientViewerRect.setHeight(size.height());
	m_clientViewerMap.erase(userID);
}

void PandaRC::loginReq()
{
	NSPROTO::LOGIN_REQ login;
	login.userid = 0;
	std::string strMac = NSMisc::getHostMac();
	strcpy(login.mac_addr, strMac.c_str());
	m_netThread->sendMsg(0, ENET_PACKET_FLAG_RELIABLE, &login);
}

void PandaRC::onLoginRet(NSPROTO::LOGIN_RET* proto)
{
	m_nUserID = proto->userid;
}

void PandaRC::onFrameSync(NSPROTO::FRAME_SYNC* proto)
{
	ViewerIter iter = m_clientViewerMap.find(proto->srv_uid);
	if (iter == m_clientViewerMap.end())
	{
		return;
	}
	iter->second->onFrameSync(proto);
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
	if (service == 1)
	{
		m_asServerRoomMap[roomID] = time(NULL);
		if (m_handlerSrv == NULL)
		{
			m_handlerSrv = new UpdateHandlerServer(this);
		}
	}
	else if (service == 2)
	{
		m_asClientRoomMap[roomID] = time(NULL);
	}
}

void PandaRC::onUnbuildRet(int roomID, int service)
{
	if (service == 1)
	{
		m_asServerRoomMap.erase(roomID);
		if (m_asServerRoomMap.size() <= 0)
		{
			SAFE_DELETE(m_handlerSrv);
		}
	}
	else if (service == 2)
	{
		m_asClientRoomMap.erase(roomID);
	}
}

void PandaRC::connectedCallback(void* param)
{
	PandaRC* pPandaRC = (PandaRC*)param;
	pPandaRC->loginReq();
}

