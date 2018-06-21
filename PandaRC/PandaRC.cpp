#include "PandaRC.h"

PandaRC::PandaRC(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	Logger::Instance()->Init();
	m_pUDPNet = new UDPNet(parent);
}

void PandaRC::onBtnSend()
{
}

void PandaRC::onBtnRecv()
{
}
