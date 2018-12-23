#include "GLClientViewer.h"
#include "PandaRC.h"
#include "Misc.h"

GLClientViewer::GLClientViewer(QWidget *parent)
	: QGLWidget()
{
	ui.setupUi(this);
	m_parent = parent;
	m_pixmap = NULL;
	m_painter = NULL;

	m_roomID = 0;
	m_userID = 0;
	
	PandaRC* pandaRC = (PandaRC*)m_parent;
	connect(pandaRC->getFrameThread(), SIGNAL(paintDataChanged()), this, SLOT(onPaintDataChanged()));
	setAttribute(Qt::WA_DeleteOnClose, true);
}

GLClientViewer::~GLClientViewer()
{
	PandaRC* pandaRC = (PandaRC*)m_parent;
	disconnect(pandaRC->getFrameThread(), SIGNAL(paintDataChanged()), this, SLOT(onPaintDataChanged()));
}

void GLClientViewer::initializeGL()
{
	glShadeModel(GL_FLAT);//设置阴影平滑模式
	glClearColor(0xFF, 0xFF, 0xFF, 0xFF);//改变窗口的背景颜色，不过我这里貌似设置后并没有什么效果
	glClearDepth(1.0);//设置深度缓存
	glEnable(GL_DEPTH_TEST);//允许深度测试
	glDepthFunc(GL_LEQUAL);//设置深度测试类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正
}

void GLClientViewer::resizeGL(int w, int h)
{
	if (h == 0)
	{
		h = 1;//防止一条边为0
	}
	glViewport(0, 0, (GLint)w, (GLint)h);//重置当前视口，本身不是重置窗口的，只不过是这里被Qt给封装好了
	glMatrixMode(GL_PROJECTION);//选择投影矩阵
	glLoadIdentity();//重置选择好的投影矩阵
 // gluPerspective(45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0);//建立透视投影矩阵
 //	glMatirxMode(GL_MODELVIEW);//以下2句和上面出现的解释一样
	glLoadIdentity();
}

void GLClientViewer::paintEvent(QPaintEvent *event)
{
	if (m_pixmap == NULL)
	{
		return;
	}
	QPainter painter;
	painter.begin(this);

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
	painter.end();
}

void GLClientViewer::closeEvent(QCloseEvent *event)
{
	PandaRC* pandaRC = (PandaRC*)m_parent;
	pandaRC->onViewerClose(m_userID);
}

void GLClientViewer::onFrameSync(NSPROTO::FRAME_SYNC* proto)
{
	PandaRC* pandaRC = (PandaRC*)m_parent;
	int frameSize = sizeof(NSPROTO::FRAME_SYNC::FRAME) - sizeof(uint8_t*);

	int offsetSize = 0;
	for (int i = 0; i < proto->frameNum; i++)
	{
		NSPROTO::FRAME_SYNC::FRAME* frame = (NSPROTO::FRAME_SYNC::FRAME*)((uchar*)(&proto->dataPtr) + offsetSize);

		PDFRAME* pd = new PDFRAME(PDEVENT_TYPE::eFRAME);
		pd->rect.left = frame->left;
		pd->rect.top = frame->top;
		pd->rect.setWidth(frame->width);
		pd->rect.setHeight(frame->height);
		pd->fb.setDimension(&Dimension(frame->width, frame->height));

		PixelFormat pixFmt = pd->fb.getPixelFormat();
		pixFmt.bitsPerPixel = frame->bitsPerPixel;
		pd->fb.setPixelFormat(&pixFmt);

		int uncompressSize = 0;
		uint8_t* uncompressBuffer = NULL;
		bool res = NSMisc::zipUncompress((uchar*)(&frame->bufferPtr), frame->bufferSize, &uncompressBuffer, uncompressSize);
		if (res)
		{
			uchar* buffer = new uchar[uncompressSize];
			memcpy(buffer, uncompressBuffer, uncompressSize);
			pd->fb.setBuffer((void*)buffer);
			pandaRC->getFrameThread()->addClientFrame(pd);
		}
		else
		{
			XLog(LEVEL_INFO, "uncompress fail %d->%d\n", res, frame->bufferSize, uncompressSize);
		}

		offsetSize += frameSize + frame->bufferSize;
	}
}

void GLClientViewer::onPaintDataChanged()
{
	PandaRC* pandaRC = (PandaRC*)m_parent;
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
				QImage oImg(pBuffer, frame->rect.getWidth(), frame->rect.getHeight(), nBytesRow, QImage::Format_RGB16);
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
	update();
}
