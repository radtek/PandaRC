#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PandaRC.h"

#include "QFrameThread.h"
#include "QNetThread.h"
#include "ClientViewer.h"
#include "GLClientViewer.h"

#include <list>
#include <unordered_map>

class UpdateHandlerServer;

class PandaRC : public QMainWindow
{
	Q_OBJECT

public:
	typedef std::unordered_map<int, int> RoomMap;
	typedef RoomMap::iterator RoomIter;

	typedef std::unordered_map<int, GLClientViewer*> ViewerMap;
	typedef ViewerMap::iterator ViewerIter;

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

private:
	Ui::PandaRCClass ui;

public:
	QNetThread* getNetThread() { return m_netThread; }
	QFrameThread* getFrameThread() { return m_frameThread; }
	///////////////��������//////////////
	void loginReq();
	void buildReq();
	void onLoginRet(NSPROTO::LOGIN_RET* proto);
	void onBuildRet(int roomID, int service);
	void onUnbuildRet(int roomID, int service);
	void onFrameSync(NSPROTO::FRAME_SYNC* proto);

private:
	QPixmap* m_pixmap;
	QPainter* m_painter;
	QNetThread* m_netThread;
	QFrameThread* m_frameThread;
	UpdateHandlerServer* m_handlerSrv;

	RoomMap m_asServerRoomMap;		//��Ϊ�������ķ����
	RoomMap m_asClientRoomMap;		//��Ϊ�ͻ��˵ķ����
	ViewerMap m_clientViewerMap;	//�ͻ�����ʾ����
	QRect m_clientViewerRect;

	int m_nUserID;
};
