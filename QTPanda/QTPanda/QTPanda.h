#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QTPanda.h"

class QTPanda : public QMainWindow
{
	Q_OBJECT

public:
	QTPanda(QWidget *parent = Q_NULLPTR);

protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

public slots:
	void onBtnSendMsg();
	void onBtnInject();
	void onBtnUnject();

private:
	Ui::QTPandaClass ui;
};
