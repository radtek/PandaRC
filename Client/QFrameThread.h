#pragma once

#include <QThread>
#include <QPainter>
#include <queue>
#include "win-system/WindowsEvent.h"
#include "region/Rect.h"
#include "region/Point.h"
#include "rfb/FrameBuffer.h"
#include "thread/LocalMutex.h"

enum PDEVENT_TYPE
{
	eFRAME = 1,	
	eCURSOR = 2,
	eFRAME_T = 3,
};

class PDEVENT
{
public:
	PDEVENT(int8_t _type): type(_type) {}
	virtual ~PDEVENT() {}
	int8_t type;
};

class PDFRAME : public PDEVENT
{
public:
	PDFRAME(int8_t _type): PDEVENT(_type) {}
	virtual ~PDFRAME() {}
	int8_t type;
	Rect rect;
	FrameBuffer fb;
};

class PDFRAME_T : public PDEVENT
{
public:
	PDFRAME_T(int8_t _type): PDEVENT(_type) 
	{
		buffer = NULL;
	}
	virtual ~PDFRAME_T() 
	{
		if (buffer != NULL)
		{
			free(buffer);
			buffer = NULL;
		}
	}
	int8_t type;
	Rect rect;
	uchar* buffer;
};

class PDCURSOR : public PDEVENT
{
public:
	PDCURSOR(int8_t _type): PDEVENT(_type) {}
	virtual ~PDCURSOR() {}
	Point pos;
	bool posChange;
	
	Dimension dim;
	Point hotspot;
	char buffer[4096];
	int bufferSize;
	char mask[4096];
	int maskSize;
	bool shapeChange;
};

class QFrameThread : public QThread
{
	Q_OBJECT
public:
	explicit QFrameThread(QObject *parent = 0, QWidget* parentWidget = NULL);
	virtual ~QFrameThread();
	virtual void run();

	void updateServer();
	void updateClient();

signals:
	void paintDataChanged();

public:
	void notify() { m_updateTimeout.notify(); }
	void addServerFrame(PDEVENT* pd);
	void addClientFrame(PDEVENT* pd);
	PDEVENT* getClientFrame();

protected:
	WindowsEvent m_updateTimeout;
	std::queue<PDEVENT*> m_pdServerList;
	std::queue<PDEVENT*> m_pdClientList;
	std::queue<PDEVENT*> m_oneClientFrame;

	LocalMutex m_pdMutex;
	QWidget* m_parentWidget;

	QPixmap* m_pixmap;
	QPainter* m_painter;
};
