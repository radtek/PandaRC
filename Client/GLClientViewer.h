#pragma once

#include <QGLWidget>
#include "ui_GLClientViewer.h"

class GLClientViewer : public QGLWidget
{
	Q_OBJECT

public:
	GLClientViewer(QWidget *parent = Q_NULLPTR);
	~GLClientViewer();

private:
	Ui::GLClientViewer ui;
};
