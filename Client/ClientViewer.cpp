#include "ClientViewer.h"
#include "PandaRC.h"

ClientViewer::ClientViewer(QWidget *parent)
	: QGLWidget(parent, NULL, Qt::Window)
{
	ui.setupUi(this);
	m_painter = NULL;
	m_pixmap = NULL;

	PandaRC* pandaRC = (PandaRC*)parentWidget();
	connect(pandaRC->getFrameThread(), SIGNAL(paintDataChanged()), this, SLOT(onPaintDataChanged()));
}

ClientViewer::~ClientViewer()
{
}

void ClientViewer::closeEvent(QCloseEvent *event)
{
	PandaRC* pandaRC = (PandaRC*)parentWidget();
	pandaRC->onViewerClose(m_userID);
}

void ClientViewer::paintEvent(QPaintEvent *event)
{
	if (m_pixmap == NULL)
	{
		return;
	}
	QPainter painter(this);

	const QSize& screenSize = size();
	const QSize& bufferSize = m_pixmap->size();
	if (bufferSize != screenSize)
	{
		QPixmap scalePixmap = m_pixmap->scaled(screenSize.width(), screenSize.height());
		painter.drawPixmap(0, 0, scalePixmap);
	}
	else
	{
		painter.eraseRect(0, 0, screenSize.width(), screenSize.height());
		painter.drawPixmap(0, 0, *m_pixmap);
	}
}

void ClientViewer::onFrameSync(NSPROTO::FRAME_SYNC* proto)
{
	PandaRC* pandaRC = (PandaRC*)parentWidget();

	PDFRAME* pd = new PDFRAME(PDEVENT_TYPE::eFRAME);
	pd->rect.left = proto->left;
	pd->rect.top = proto->top;
	pd->rect.setWidth(proto->width);
	pd->rect.setHeight(proto->height);
	pd->fb.setDimension(&Dimension(proto->width, proto->height));

	PixelFormat pixFmt = pd->fb.getPixelFormat();
	pixFmt.bitsPerPixel = proto->bitsPerPixel;
	pd->fb.setPixelFormat(&pixFmt);

	uchar* buffer = new uchar[proto->bufferSize];
	memcpy(buffer, (uchar*)(&proto->dataPtr), proto->bufferSize);
	pd->fb.setBuffer((void*)buffer);
	pandaRC->getFrameThread()->addClientFrame(pd);


	//PDFRAME_T* pdt = new PDFRAME_T(PDEVENT_TYPE::eFRAME_T);
	//pdt->rect.left = proto->left;
	//pdt->rect.top = proto->top;
	//pdt->rect.setWidth(proto->width);
	//pdt->rect.setHeight(proto->height);
	//uchar* buffer = (uchar*)realloc(NULL, proto->bufferSize);
	//memcpy(buffer, (uchar*)(&proto->dataPtr), proto->bufferSize);
	//pdt->buffer = buffer;
	//pandaRC->getFrameThread()->addClientFrame(pdt);
}

void ClientViewer::onPaintDataChanged()
{
	PandaRC* pandaRC = (PandaRC*)parentWidget();
	do {
		PDEVENT* pd = pandaRC->getFrameThread()->getClientFrame();
		if (pd == NULL)
		{
			break;
		}
		switch (pd->type)
		{
			case PDEVENT_TYPE::eFRAME:
			{
				PDFRAME* frame = (PDFRAME*)pd;
				Dimension dim = frame->fb.getDimension();
				if (m_pixmap == NULL)
				{
					m_pixmap = new QPixmap(dim.width, dim.height);
					m_painter = new QPainter(m_pixmap);
				}
				int nBytesRow = frame->fb.getBytesPerRow();
				uchar* pBuffer = (uchar*)frame->fb.getBuffer();
				QImage oImg(pBuffer, frame->rect.getWidth(), frame->rect.getHeight(), nBytesRow, QImage::Format_RGB32);
				m_painter->drawImage(QPoint(frame->rect.left, frame->rect.top), oImg);
				break;
			}
			case PDEVENT_TYPE::eCURSOR:
			{
				//PDCURSOR* pdcr = (PDCURSOR*)pd;
				//const QSize& screenSize = size();
				//const QSize& bufferSize = m_pixmap->size();
				//if (pdcr->shapeChange)
				//{
				//	QPixmap pixmap(pdcr->dim.width, pdcr->dim.height);
				//	pixmap.loadFromData((const uchar*)pdcr->buffer, pdcr->bufferSize);
				//	QCursor cursor(pixmap, pdcr->hotspot.x, pdcr->hotspot.y);
				//	if (pdcr->posChange)
				//	{
				//		cursor.setPos(pdcr->pos.x, pdcr->pos.y);
				//	}
				//	else
				//	{
				//		const QCursor& orgCursor = QWidget::cursor();
				//		cursor.setPos(orgCursor.pos().x(), orgCursor.pos().y());
				//	}
				//	setCursor(cursor);
				//}
				//if (pdcr->posChange)// && !pdcr->shapeChange)
				//{
				//	double ratioX = screenSize.width() / bufferSize.width();
				//	double ratioY = screenSize.height() / bufferSize.height();
				//	SetCursorPos((int)pdcr->pos.x*ratioX, (int)pdcr->pos.y*ratioY);
				//}
				break;
			}
		}
		delete pd;

	} while (true);

	//do {
	//	PDEVENT* pd = pandaRC->getFrameThread()->getClientFrame();
	//	if (pd == NULL)
	//	{
	//		break;
	//	}
	//	switch (pd->type)
	//	{
	//		case PDEVENT_TYPE::eFRAME_T:
	//		{
	//			PDFRAME_T* frame = (PDFRAME_T*)pd;
	//			if (m_pixmap == NULL)
	//			{
	//				m_pixmap = new QPixmap(frame->rect.getWidth(), frame->rect.getHeight());
	//				m_painter = new QPainter(m_pixmap);
	//			}
	//			QImage img = QImage(frame->buffer, frame->rect.getWidth(), frame->rect.getHeight(), QImage::Format_RGB32);
	//			m_painter->drawImage(QPoint(frame->rect.left, frame->rect.top), img);
	//			break;
	//		}
	//	}
	//	delete pd;

	//} while (true);
	update();
}


void ClientViewer::initializeGL()
{
	glShadeModel(GL_FLAT);//设置阴影平滑模式
	glClearColor(0.5, 1.0, 0.2, 0);//改变窗口的背景颜色，不过我这里貌似设置后并没有什么效果
	glClearDepth(1.0);//设置深度缓存
	glEnable(GL_DEPTH_TEST);//允许深度测试
	glDepthFunc(GL_LEQUAL);//设置深度测试类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正
}

void ClientViewer::resizeGL(int w, int h)
{
	if (h == 0)
	{
		h = 1;//防止一条边为0
	}
	glViewport(0, 0, (GLint)w, (GLint)h);//重置当前视口，本身不是重置窗口的，只不过是这里被Qt给封装好了
	glMatrixMode(GL_PROJECTION);//选择投影矩阵
	glLoadIdentity();//重置选择好的投影矩阵
					 // gluPerspective(45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0);//建立透视投影矩阵
					 //glMatirxMode(GL_MODELVIEW);//以下2句和上面出现的解释一样
	glLoadIdentity();
}

void ClientViewer::paintGL()
{
	//glClear()函数在这里就是对initializeGL()函数中设置的颜色和缓存深度等起作用
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();//重置当前的模型观察矩阵？不是很理解！
}
