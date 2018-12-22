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
	m_clientViewerRect.setRect(100, 100, 800, 600);

	m_nUserID = 0;
	m_bLogin = false;

	startTimer(1000/60); //单位为毫秒
		
}

void PandaRC::onBtnLogin()
{
	if (m_bLogin)
	{
		QMessageBox::critical(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("已经登录了"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	QString text = ui.leUserID->text();
	int nUserID = text.toInt();
	if (nUserID == 0)
	{
		QMessageBox::critical(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("用户ID错误"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	m_nUserID = nUserID;
	//m_netThread->connect("192.168.95.128", 10009, PandaRC::connectedCallback, this);
	//m_netThread->connect("120.92.10.82", 10009, PandaRC::connectedCallback, this);
	m_netThread->connect("127.0.0.1", 10009, PandaRC::connectedCallback, this);
}

void PandaRC::onBtnBuild()
{
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

void PandaRC::timerEvent(QTimerEvent *event)
{
	dispatchNetMsg();
}

void PandaRC::onViewerClose(int userID)
{
	ViewerIter iter = m_clientViewerMap.find(userID);
	if (iter == m_clientViewerMap.end())
	{
		return;
	}

	GLClientViewer* pViewer = iter->second;
	QPoint pos = pViewer->pos();
	QSize size = pViewer->size();
	m_clientViewerRect.setX(pos.x());
	m_clientViewerRect.setY(pos.y());
	m_clientViewerRect.setWidth(size.width());
	m_clientViewerRect.setHeight(size.height());
	m_clientViewerMap.erase(userID);


	UnbuildReq(pViewer->getRoomID());
}

void PandaRC::UnbuildReq(int roomID)
{
	NSPROTO::UNBUILD_REQ unbuildreq;
	unbuildreq.roomid = roomID;
	m_netThread->sendMsg(0, ENET_PACKET_FLAG_RELIABLE, &unbuildreq);
}

void PandaRC::onUnbuildRet(int roomID, int service)
{
	if (service == 1) //Server
	{
		m_asServerRoomMap.erase(roomID);
		if (m_asServerRoomMap.size() <= 0)
		{
			SAFE_DELETE(m_handlerSrv);
		}
	}
	else if (service == 2) //Client
	{
		m_asClientRoomMap.erase(roomID);
	}
}

void PandaRC::loginReq()
{
	NSPROTO::LOGIN_REQ login;
	login.userid = m_nUserID;
	std::string strMac = NSMisc::getHostMac();
	strcpy(login.mac_addr, strMac.c_str());
	m_netThread->sendMsg(0, ENET_PACKET_FLAG_RELIABLE, &login);
}

void PandaRC::onLoginRet(NSPROTO::LOGIN_RET* proto)
{
	m_bLogin = true;
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
	build.client_userid = m_nUserID;
	m_netThread->sendMsg(0, ENET_PACKET_FLAG_RELIABLE, &build);
}

void PandaRC::onBuildRet(int roomID, int service)
{
	if (service == 1) //Server
	{
		m_asServerRoomMap[roomID] = time(NULL);
		if (m_handlerSrv == NULL)
		{
			m_handlerSrv = new UpdateHandlerServer(this);
		}
	}
	else if (service == 2) //Client
	{
		if (m_asClientRoomMap.find(roomID) != m_asClientRoomMap.end())
		{
			XLog(LEVEL_ERROR, "onBuildRet room:%d already exist\n", roomID);
			return;
		}
		m_asClientRoomMap[roomID] = time(NULL);
		GLClientViewer* pView = new GLClientViewer(this);
		m_clientViewerMap[1] = pView;
		pView->setUserID(1);
		pView->setRoomID(roomID);

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
}

void PandaRC::connectedCallback(void* param)
{
	PandaRC* pPandaRC = (PandaRC*)param;
	pPandaRC->loginReq();
}


void PandaRC::onReceiveNetMsg(NETMSG* msg)
{
	AutoLock al(&m_netMsgMutex);
	m_netMsgList.push_back(msg);
}

void PandaRC::dispatchNetMsg()
{
	AutoLock al(&m_netMsgMutex);
	if (m_netMsgList.size() <= 0)
	{
		return;
	}

	NETMSG* msg = m_netMsgList.front();
	m_netMsgList.pop_front();

	NSPROTO::HEAD head = *(NSPROTO::HEAD*)msg->data;
	switch (head.cmd)
	{
		case NSNETCMD::eLOGIN_RET:
		{
			NSPROTO::LOGIN_RET loginret = *(NSPROTO::LOGIN_RET*)msg->data;
			if (loginret.code == 0)
			{
				XLog(LEVEL_INFO, "Login successful:%d\n", loginret.userid);
				onLoginRet(&loginret);
			}
			else
			{
				XLog(LEVEL_INFO, "Login fail\n");
			}
			break;
		}
		case NSNETCMD::eBUILD_RET:
		{
			NSPROTO::BUILD_RET buildret = *(NSPROTO::BUILD_RET*)msg->data;
			XLog(LEVEL_INFO, "Build room ret roomid:%d\n", buildret.roomid);
			if (buildret.roomid > 0)
			{
				onBuildRet(buildret.roomid, buildret.service);
			}
			break;
		}
		case NSNETCMD::eUNBUILD_RET:
		{
			NSPROTO::UNBUILD_RET unbuildret = *(NSPROTO::UNBUILD_RET*)msg->data;
			XLog(LEVEL_INFO, "Unbuild room ret roomid:%d\n", unbuildret.roomid);
			if (unbuildret.code == 0)
			{
				onUnbuildRet(unbuildret.roomid, unbuildret.service);
			}
			break;
		}
		case NSNETCMD::eFRAME_SYNC:
		{
			NSPROTO::FRAME_SYNC* framesync = (NSPROTO::FRAME_SYNC*)msg->data;
			onFrameSync(framesync);
			break;
		}
		default:
		{
			break;
		}
	}
	SAFE_DELETE(msg);
}
