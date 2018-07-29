#include "PandaRC.h"
#include "desktop/Win32ScreenDriverFactory.h"

ScreenDriver* poDriver;
Win32ScreenDriverFactory* poDriverFactory;

PandaRC::PandaRC(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	Logger::Instance()->Init();
	m_pUDPNet = new UDPNet(parent);

	poDriverFactory = new Win32ScreenDriverFactory();
	poDriver = poDriverFactory->createScreenDriver();
	
	//³ß´ç
	Dimension oDimension = poDriver->getScreenDimension();
	FrameBuffer* pScreenBuffer = poDriver->getScreenBuffer();
	Rect oFb;
	poDriver->grabFb(&oFb);
	bool bChanged1 = poDriver->getScreenPropertiesChanged();
	bool bChanged2 = poDriver->getScreenSizeChanged();
	int i = 0;
}

void PandaRC::onBtnSend()
{
}

void PandaRC::onBtnRecv()
{
}
