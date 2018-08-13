#include "PandaRC.h"
#include <qpainter.h>
#include <qmath.h>

#include "log-writer/LogWriter.h"
#include "desktip-ipc/UpdateHandlerServer.h"

LogWriter glog;
UpdateHandlerServer* pHandlerSrv;


PandaRC::PandaRC(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//Logger::Instance()->Init();
	m_pUDPNet = new UDPNet(parent);
	pHandlerSrv = new UpdateHandlerServer(this);
}

void PandaRC::onBtnSend()
{
}

void PandaRC::onBtnRecv()
{
}

void PandaRC::paintEvent(QPaintEvent *event)
{
	static int i = 0;
	if (i == 0)
	{
		std::vector<FrameBuffer*>& fbList = pHandlerSrv->getUpdateFrameList();
		for (int i = 0; i < fbList.size(); i++)
		{
			FrameBuffer* fb = fbList[i];
			Dimension dim = fb->getDimension();
			uchar* pBuffer = (uchar*)fb->getBuffer();
			int nBuffSize = fb->getBufferSize();
			int nBytesRow = fb->getBytesPerRow();
			QImage oImg(pBuffer, dim.width, dim.height, nBytesRow, QImage::Format_RGB32);
			printf("img is null: %d\n", oImg.isNull());
			QPainter painter(this);
			painter.drawImage(QPoint(0, 0), oImg);
			delete fb;
		}
		fbList.clear();
		i++;
	}

	//poDriver->grabFb();
	//HWND m_hWnd = (HWND)winId();
	//pScreenBuffer = poDriver->getScreenBuffer();
	//Dimension oDim = poDriver->getScreenDimension();
	//uchar* pBuffer = (uchar*)pScreenBuffer->getBuffer();
	//int nBuffSize = pScreenBuffer->getBufferSize();
	//int nBytesRow = pScreenBuffer->getBytesPerRow();
	//QImage oImg(pBuffer, oDim.width, oDim.height, nBytesRow, QImage::Format_RGB32);
	//printf("img is null: %d\n", oImg.isNull());
	//QPainter painter(this);
	//painter.drawImage(QPoint(0, 0), oImg);
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
