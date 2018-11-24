#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PandaRC.h"

#include "QFrameThread.h"
#include "QNetThread.h"

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
	QNetThread* getNetThread() { return &m_netThread; }
	QFrameThread* getFrameThread() { return m_frameThread; }

private:
	QPixmap* m_pixmap;
	QPainter* m_painter;
	QNetThread m_netThread;
	QFrameThread* m_frameThread;
};
