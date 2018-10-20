#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PandaRC.h"

#include "UDPNet/UDPNet.h"
#include "QMyThread.h"

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
	QMyThread* getUpdateThread() { return &m_oMyThread; }

private:
	UDPNet* m_pUDPNet;
	QMyThread m_oMyThread;

	QPixmap* m_nPixmap;
	QPainter* m_nPainter;
};
