#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QTPanda.h"
#include "Common/XThread.h"

class QTPanda : public QMainWindow
{
	Q_OBJECT

public:
	QTPanda(QWidget *parent = Q_NULLPTR);
	static void WorkderFunc(void* param);

public slots:
	void onBtnSendMsg();
	void onBtnInject();
	void onBtnUnject();

protected:
	void closeEvent(QCloseEvent *event);
	void injectDll(LPCSTR lpszProcessName, LPCSTR lpszDllName);
	void unjectDll(LPCSTR lpszProcessName, LPCSTR lpszDllName);

private:
	Ui::QTPandaClass ui;
	XThread m_oWorderThread;
	bool m_bShutDown;
};
