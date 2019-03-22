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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QTPandaClass
{
public:
    QWidget *centralWidget;
    QPushButton *btnInject;
    QPushButton *btnUnject;
    QPushButton *btnSendMsg;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QTPandaClass)
    {
        if (QTPandaClass->objectName().isEmpty())
            QTPandaClass->setObjectName(QStringLiteral("QTPandaClass"));
        QTPandaClass->resize(600, 400);
        centralWidget = new QWidget(QTPandaClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btnInject = new QPushButton(centralWidget);
        btnInject->setObjectName(QStringLiteral("btnInject"));
        btnInject->setGeometry(QRect(20, 10, 81, 31));
        btnUnject = new QPushButton(centralWidget);
        btnUnject->setObjectName(QStringLiteral("btnUnject"));
        btnUnject->setGeometry(QRect(20, 50, 81, 31));
        btnSendMsg = new QPushButton(centralWidget);
        btnSendMsg->setObjectName(QStringLiteral("btnSendMsg"));
        btnSendMsg->setGeometry(QRect(20, 100, 81, 31));
        QTPandaClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QTPandaClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        QTPandaClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QTPandaClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QTPandaClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QTPandaClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QTPandaClass->setStatusBar(statusBar);

        retranslateUi(QTPandaClass);
        QObject::connect(btnInject, SIGNAL(clicked()), QTPandaClass, SLOT(onBtnInject()));
        QObject::connect(btnUnject, SIGNAL(clicked()), QTPandaClass, SLOT(onBtnUnject()));
        QObject::connect(btnSendMsg, SIGNAL(clicked()), QTPandaClass, SLOT(onBtnSendMsg()));

        QMetaObject::connectSlotsByName(QTPandaClass);
    } // setupUi

    void retranslateUi(QMainWindow *QTPandaClass)
    {
        QTPandaClass->setWindowTitle(QApplication::translate("QTPandaClass", "QTPanda", nullptr));
        btnInject->setText(QApplication::translate("QTPandaClass", "\346\263\250\345\205\245", nullptr));
        btnUnject->setText(QApplication::translate("QTPandaClass", "\345\215\270\350\275\275", nullptr));
        btnSendMsg->setText(QApplication::translate("QTPandaClass", "\346\266\210\346\201\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QTPandaClass: public Ui_QTPandaClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTPANDA_H
