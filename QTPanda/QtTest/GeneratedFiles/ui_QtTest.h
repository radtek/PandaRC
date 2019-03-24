/********************************************************************************
** Form generated from reading UI file 'QtTest.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTTEST_H
#define UI_QTTEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtTestClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtTestClass)
    {
        if (QtTestClass->objectName().isEmpty())
            QtTestClass->setObjectName(QStringLiteral("QtTestClass"));
        QtTestClass->resize(600, 400);
        menuBar = new QMenuBar(QtTestClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        QtTestClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtTestClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtTestClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtTestClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QtTestClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtTestClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtTestClass->setStatusBar(statusBar);

        retranslateUi(QtTestClass);

        QMetaObject::connectSlotsByName(QtTestClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtTestClass)
    {
        QtTestClass->setWindowTitle(QApplication::translate("QtTestClass", "QtTest", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtTestClass: public Ui_QtTestClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTTEST_H
