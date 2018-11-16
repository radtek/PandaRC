#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PandaRC.h"

#include "QMyThread.h"
#include "Common/Network/Network.h"

class PandaRC : public QMainWindow
{
	Q_OBJECT

public:
	PandaRC(QWidget *parent = Q_NULLPTR);

public slots:
	void onBtnSend();
	void onBtnRecv();
	void onPaintDataChanged();

protected:
	void paintEvent(QPaintEvent *event);

private:
	Ui::PandaRCClass ui;

public:
	QMyThread* getUpdateThread() { return &m_myThread; }

private:
	QMyThread m_myThread;
	QPixmap* m_pixmap;
	QPainter* m_painter;

	Network m_oNetwork;
};
