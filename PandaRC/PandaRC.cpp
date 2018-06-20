#include "PandaRC.h"

PandaRC::PandaRC(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_pUDPNet = new UDPNet(parent);
}

void PandaRC::onBtnSend()
{
	XLog(LEVEL_INFO, "hello button******\n");
}
