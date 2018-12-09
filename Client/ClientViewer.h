#pragma once

#include "Include/Logger/Logger.hpp"
#include "Common/Network/Protocol.h"

#include <QGLWidget>
#include "ui_ClientViewer.h"

struct DISPDATA
{
	DISPDATA(QImage* _img, const QRect& _rect)
	{
		img = _img;
		rect = _rect;
	}
	~DISPDATA()
	{
		SAFE_DELETE(img);
	}
	QImage* img;
	QRect rect;
};

class ClientViewer : public QWidget
{
	Q_OBJECT

public:
	ClientViewer(QWidget *parent = Q_NULLPTR);
	~ClientViewer();

public:
	void setUserID(int userID) { m_userID = userID;  }
	int getUserID() { return m_userID; }
	void onFrameSync(NSPROTO::FRAME_SYNC* proto);

protected:
	void paintEvent(QPaintEvent *event);
	void closeEvent(QCloseEvent *event);

	//void initializeGL(); //��ʼ����ǰ��opengl����
	//void resizeGL(int w, int h); //��Ҫ��������Ⱦ����ĸ߶ȺͿ�Ƚ���һЩ�仯����
	//void paintGL(); //����

public slots:
	void onPaintDataChanged();

private:
	Ui::ClientViewer ui;
	int m_userID;

	QPixmap* m_pixmap;
	QPainter* m_painter;
	std::queue<DISPDATA*> m_dispData;
};
