#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PandaRC.h"

#include "QFrameThread.h"
#include "QNetThread.h"
#include "ClientViewer.h"
#include "GLClientViewer.h"

#include <list>
#include <unordered_map>

//网络包
struct NETMSG
{
	uint8_t* data;
	int size;

	NETMSG(uint8_t* _data, int _size)
	{
		data = (uint8_t*)XALLOC(NULL, _size);
		memcpy(data, _data, _size);
		size = _size;
	}

	~NETMSG()
	{
		SAFE_FREE(data);
		size = 0;
	}
};

class UpdateHandlerServer;
class PandaRC : public QMainWindow
{
	Q_OBJECT

public:
	typedef std::unordered_map<int, int> RoomMap;
	typedef RoomMap::iterator RoomIter;

	typedef std::unordered_map<int, GLClientViewer*> ViewerMap;
	typedef ViewerMap::iterator ViewerIter;

	typedef std::list<NETMSG*> NetMsgList;
	typedef NetMsgList::iterator NetMsgIter;

public:
	PandaRC(QWidget *parent = Q_NULLPTR);
	void onViewerClose(int userID);
	static void connectedCallback(void* param);
	int getUserID() { return m_nUserID; }

public slots:
	void onBtnLogin();
	void onBtnBuild();

protected:
	void paintEvent(QPaintEvent *event);
	void closeEvent(QCloseEvent *event);
	void timerEvent(QTimerEvent *event);

private:
	Ui::PandaRCClass ui;

public:
	QNetThread* getNetThread() { return m_netThread; }
	QFrameThread* getFrameThread() { return m_frameThread; }

	///////////////网络请求//////////////
	void onReceiveNetMsg(NETMSG* msg);
	void dispatchNetMsg();

	void loginReq();
	void onLoginRet(NSPROTO::LOGIN_RET* proto);

	void buildReq();
	void onBuildRet(int roomID, int service);

	void UnbuildReq(int roomID);
	void onUnbuildRet(int roomID, int service);

	void onFrameSync(NSPROTO::FRAME_SYNC* proto);

private:
	QPixmap* m_pixmap;
	QPainter* m_painter;
	QNetThread* m_netThread;
	QFrameThread* m_frameThread;
	UpdateHandlerServer* m_handlerSrv;

	RoomMap m_asServerRoomMap;		//作为服务器的房间表
	RoomMap m_asClientRoomMap;		//作为客户端的房间表
	ViewerMap m_clientViewerMap;	//客户端显示窗口
	QRect m_clientViewerRect;

	int m_nUserID;
	bool m_bLogin;

	NetMsgList m_netMsgList;
	LocalMutex m_netMsgMutex;
};
