#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PandaRC.h"

#include "enet/enet.h"
#include "QMyThread.h"
#include "UDPNet/UDPNet.h"

class PandaRC : public QMainWindow
{
	Q_OBJECT

public:
	PandaRC(QWidget *parent = Q_NULLPTR);
	bool initENet();

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
	UDPNet* m_pUDPNet;

	ENetHost* m_enetClient;
	ENetPeer* m_enetPeer;

	QMyThread m_myThread;
	QPixmap* m_pixmap;
	QPainter* m_painter;
};
