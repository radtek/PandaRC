/********************************************************************************
** Form generated from reading UI file 'PandaRC.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PANDARC_H
#define UI_PANDARC_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PandaRCClass
{
public:
    QWidget *centralWidget;
    QPushButton *btnSend;
    QPushButton *btnRecv;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PandaRCClass)
    {
        if (PandaRCClass->objectName().isEmpty())
            PandaRCClass->setObjectName(QStringLiteral("PandaRCClass"));
        PandaRCClass->resize(600, 400);
        centralWidget = new QWidget(PandaRCClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btnSend = new QPushButton(centralWidget);
        btnSend->setObjectName(QStringLiteral("btnSend"));
        btnSend->setGeometry(QRect(40, 20, 75, 23));
        btnRecv = new QPushButton(centralWidget);
        btnRecv->setObjectName(QStringLiteral("btnRecv"));
        btnRecv->setGeometry(QRect(40, 70, 75, 23));
        PandaRCClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(PandaRCClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        PandaRCClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(PandaRCClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        PandaRCClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(PandaRCClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        PandaRCClass->setStatusBar(statusBar);

        retranslateUi(PandaRCClass);
        QObject::connect(btnSend, SIGNAL(clicked()), PandaRCClass, SLOT(onBtnSend()));

        QMetaObject::connectSlotsByName(PandaRCClass);
    } // setupUi

    void retranslateUi(QMainWindow *PandaRCClass)
    {
        PandaRCClass->setWindowTitle(QApplication::translate("PandaRCClass", "PandaRC", nullptr));
        btnSend->setText(QApplication::translate("PandaRCClass", "\345\217\221\351\200\201", nullptr));
        btnRecv->setText(QApplication::translate("PandaRCClass", "\346\216\245\346\224\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PandaRCClass: public Ui_PandaRCClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PANDARC_H
