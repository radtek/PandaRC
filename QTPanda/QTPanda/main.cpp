#include "QTPanda.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTPanda w;
	w.show();
	return a.exec();
}
