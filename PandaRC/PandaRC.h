#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PandaRC.h"
#include "UDPNet\UDPNet.h"

class PandaRC : public QMainWindow
{
	Q_OBJECT

public:
	PandaRC(QWidget *parent = Q_NULLPTR);

public slots:
	void onBtnSend();

private:
	Ui::PandaRCClass ui;

private:
	UDPNet* m_pUDPNet;
};
