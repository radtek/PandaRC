#include "ClientViewer.h"
#include "PandaRC.h"

ClientViewer::ClientViewer(QWidget *parent)
	: QWidget(parent, Qt::Window)
{
	ui.setupUi(this);
}

ClientViewer::~ClientViewer()
{
}

void ClientViewer::closeEvent(QCloseEvent *event)
{
	PandaRC* pPandaRC = (PandaRC*)parentWidget();
	pPandaRC->onViewerClose(m_key);
}
