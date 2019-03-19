/********************************************************************************
** Form generated from reading UI file 'QTPanda.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTPANDA_H
#define UI_QTPANDA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QTPandaClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QTPandaClass)
    {
        if (QTPandaClass->objectName().isEmpty())
            QTPandaClass->setObjectName(QStringLiteral("QTPandaClass"));
        QTPandaClass->resize(600, 400);
        menuBar = new QMenuBar(QTPandaClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        QTPandaClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QTPandaClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QTPandaClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QTPandaClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QTPandaClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QTPandaClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QTPandaClass->setStatusBar(statusBar);

        retranslateUi(QTPandaClass);

        QMetaObject::connectSlotsByName(QTPandaClass);
    } // setupUi

    void retranslateUi(QMainWindow *QTPandaClass)
    {
        QTPandaClass->setWindowTitle(QApplication::translate("QTPandaClass", "QTPanda", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QTPandaClass: public Ui_QTPandaClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTPANDA_H
