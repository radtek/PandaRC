/********************************************************************************
** Form generated from reading UI file 'GLClientViewer.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GLCLIENTVIEWER_H
#define UI_GLCLIENTVIEWER_H

#include <QtCore/QVariant>
#include <QtOpenGL/QGLWidget>
#include <QtWidgets/QApplication>

QT_BEGIN_NAMESPACE

class Ui_GLClientViewer
{
public:

    void setupUi(QGLWidget *GLClientViewer)
    {
        if (GLClientViewer->objectName().isEmpty())
            GLClientViewer->setObjectName(QStringLiteral("GLClientViewer"));
        GLClientViewer->resize(478, 373);

        retranslateUi(GLClientViewer);

        QMetaObject::connectSlotsByName(GLClientViewer);
    } // setupUi

    void retranslateUi(QGLWidget *GLClientViewer)
    {
        GLClientViewer->setWindowTitle(QApplication::translate("GLClientViewer", "GLClientViewer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GLClientViewer: public Ui_GLClientViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GLCLIENTVIEWER_H
