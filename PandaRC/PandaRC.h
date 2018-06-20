#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PandaRC.h"

class PandaRC : public QMainWindow
{
	Q_OBJECT

public:
	PandaRC(QWidget *parent = Q_NULLPTR);

private:
	Ui::PandaRCClass ui;
};
