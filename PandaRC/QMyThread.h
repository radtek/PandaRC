#pragma once

#include <QThread>
#include <queue>
#include "win-system/WindowsEvent.h"
#include "region/Rect.h"
#include "region/Point.h"
#include "rfb/FrameBuffer.h"
#include "thread/LocalMutex.h"

struct PDFRAME
{
	Rect rect;
	FrameBuffer fb;
};

struct PDCURSOR
{
	bool posChange;
	Point pos;
	
	bool shapeChange;
	Dimension dim;
	Point hotspot;
	char buffer[1024];
	int bufferSize;
	char mask[1024];
	int maskSize;
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
	void addFrame(PDFRAME* pd);
	PDFRAME* getFrame();
	PDFRAME* peekFrame();

protected:
	WindowsEvent m_updateTimeout;
	std::queue<PDFRAME*> m_pdList;
	std::queue<PDFRAME*> m_oneFrame;
	LocalMutex m_pdMutex;

};
