#include "QTPanda.h"
#include "DllInject.h"

QTPanda::QTPanda(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);


	InjectDll("memuheadless.exe", "F:\\Test\\QTPanda\\Debug\\HACK.dll");
	UnjectDll("memuheadless.exe", "HACK.dll");
}
