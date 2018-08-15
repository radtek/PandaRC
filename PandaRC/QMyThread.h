#pragma once

#include <QThread>
#include <queue>
#include "win-system/WindowsEvent.h"
#include "region/Rect.h"
#include "rfb/FrameBuffer.h"
#include "thread/LocalMutex.h"

struct PDFRAME
{
	Rect rect;
	FrameBuffer fb;
};

class QMyThread : public QThread
{
	Q_OBJECT
public:
	explicit QMyThread(QObject *parent = 0);
	virtual void run();

signals:
	void paintDataChanged(PDFRAME* pd);

public:
	void notify() { m_updateTimeout.notify(); }
	void addFrame(PDFRAME* pd);

protected:
	WindowsEvent m_updateTimeout;
	std::queue<PDFRAME*> m_pdList;
	LocalMutex m_pdMutex;

};
