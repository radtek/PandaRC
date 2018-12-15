#pragma once

#include "Include/Logger/Logger.hpp"
#include "Common/Network/Protocol.h"

#include <QGLWidget>
#include "ui_GLClientViewer.h"

class GLClientViewer : public QGLWidget
{
	Q_OBJECT

public:
	GLClientViewer(QWidget *parent = Q_NULLPTR);
	~GLClientViewer();

public:
	void setUserID(int userID) { m_userID = userID;  }
	int getUserID() { return m_userID; }
	void onFrameSync(NSPROTO::FRAME_SYNC* proto);

protected:
	void paintEvent(QPaintEvent *event);
	void closeEvent(QCloseEvent *event);
	void initializeGL(); //��ʼ����ǰ��opengl����
	void resizeGL(int w, int h); //��Ҫ��������Ⱦ����ĸ߶ȺͿ�Ƚ���һЩ�仯����

public slots:
	void onPaintDataChanged();

private:
	Ui::GLClientViewer ui;
	QWidget* m_parent;

	int m_userID;
	QPixmap* m_pixmap;
	QPainter* m_painter;
};
