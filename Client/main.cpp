#include "PandaRC.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PandaRC w;
	w.show();
	return a.exec();
}
