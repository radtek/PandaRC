#include "GLClientViewer.h"

GLClientViewer::GLClientViewer(QWidget *parent)
	: QGLWidget(parent)
{
	ui.setupUi(this);
}

GLClientViewer::~GLClientViewer()
{
}
