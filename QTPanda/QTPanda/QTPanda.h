#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QTPanda.h"

class QTPanda : public QMainWindow
{
	Q_OBJECT

public:
	QTPanda(QWidget *parent = Q_NULLPTR);

private:
	Ui::QTPandaClass ui;
};
