#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PandaRC.h"

#include "QFrameThread.h"
#include "QNetThread.h"
#include "ClientViewer.h"

#include <list>
#include <unordered_map>

class UpdateHandlerServer;

class PandaRC : public QMainWindow
{
	Q_OBJECT

public:
	typedef std::list<int> RoomList;
	typedef RoomList::iterator RoomIter;

	typedef std::unordered_map<std::string, ClientViewer*> ViewerMap;
	typedef ViewerMap::iterator ViewerIter;

public:
	PandaRC(QWidget *parent = Q_NULLPTR);
	void onViewerClose(const std::string& key);
	static void connectedCallback(void* param);

public slots:
	void onBtnLogin();
	void onBtnBuild();
	void onPaintDataChanged();

protected:
	void paintEvent(QPaintEvent *event);
	void closeEvent(QCloseEvent *event);

private:
	Ui::PandaRCClass ui;

public:
	QNetThread* getNetThread() { return m_netThread; }
	QFrameThread* getFrameThread() { return m_frameThread; }
	///////////////网络请求//////////////
	void loginReq();
	void buildReq();
	void onLoginRet(NSPROTO::PROTO* proto);
	void onBuildRet(int roomID, int service);
	void onUnbuildRet(int roomID, int service);

private:
	QPixmap* m_pixmap;
	QPainter* m_painter;
	QNetThread* m_netThread;
	QFrameThread* m_frameThread;
	UpdateHandlerServer* m_handlerSrv;

	RoomList m_clientRoomList;		//作为服务器时用
	ViewerMap m_clientViewerMap;	//作为客户端时用
	QRect m_clientViewerRect;

	int m_nUserID;
};
