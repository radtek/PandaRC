#pragma once

#include <QThread>
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
};

class PDEVENT
{
public:
	PDEVENT(int8_t _type): type(_type) {}
	int8_t type;
};

class PDFRAME : public PDEVENT
{
public:
	PDFRAME(int8_t _type): PDEVENT(_type) {}
	Rect rect;
	FrameBuffer fb;
};

class PDCURSOR : public PDEVENT
{
public:
	PDCURSOR(int8_t _type): PDEVENT(_type) {}
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

class QMyThread : public QThread
{
	Q_OBJECT
public:
	explicit QMyThread(QObject *parent = 0);
	virtual void run();

signals:
	void paintDataChanged();

public:
	void notify() { m_updateTimeout.notify(); }
	void addFrame(PDEVENT* pd);
	PDEVENT* getFrame();
	PDEVENT* peekFrame();

protected:
	WindowsEvent m_updateTimeout;
	std::queue<PDEVENT*> m_pdList;
	std::queue<PDEVENT*> m_oneFrame;
	LocalMutex m_pdMutex;

};
