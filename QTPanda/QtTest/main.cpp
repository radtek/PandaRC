#include "QtTest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtTest w;
	w.show();
	return a.exec();
}
