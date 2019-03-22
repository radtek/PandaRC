#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtTest.h"

class QtTest : public QMainWindow
{
	Q_OBJECT

public:
	QtTest(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtTestClass ui;
};
