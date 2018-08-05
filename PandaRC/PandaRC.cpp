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

void PandaRC::paintEvent(QPaintEvent *event)
{
	int i = 0;
}

//void PandaRC::onPaint(DeviceContext *dc, PAINTSTRUCT *paintStruct)
//{
//	Rect paintRect(&paintStruct->rcPaint);
//
//	if (paintRect.area() != 0) {
//		try {
//			AutoLock al(&m_bufferLock);
//			m_framebuffer.setTargetDC(paintStruct->hdc);
//			if (!m_clientArea.isEmpty()) {
//				doDraw(dc);
//			}
//		}
//		catch (const Exception &ex) {
//			m_logWriter->error(_T("Error in onPaint: %s"), ex.getMessage());
//		}
//	}
//}
