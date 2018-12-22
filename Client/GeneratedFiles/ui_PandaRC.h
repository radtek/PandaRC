/********************************************************************************
** Form generated from reading UI file 'PandaRC.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PANDARC_H
#define UI_PANDARC_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PandaRCClass
{
public:
    QWidget *centralWidget;
    QPushButton *btnLogin;
    QPushButton *btnBuild;
    QLineEdit *leUserID;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PandaRCClass)
    {
        if (PandaRCClass->objectName().isEmpty())
            PandaRCClass->setObjectName(QStringLiteral("PandaRCClass"));
        PandaRCClass->resize(229, 374);
        centralWidget = new QWidget(PandaRCClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btnLogin = new QPushButton(centralWidget);
        btnLogin->setObjectName(QStringLiteral("btnLogin"));
        btnLogin->setGeometry(QRect(50, 80, 131, 41));
        btnBuild = new QPushButton(centralWidget);
        btnBuild->setObjectName(QStringLiteral("btnBuild"));
        btnBuild->setGeometry(QRect(50, 140, 131, 41));
        leUserID = new QLineEdit(centralWidget);
        leUserID->setObjectName(QStringLiteral("leUserID"));
        leUserID->setGeometry(QRect(50, 49, 131, 21));
        PandaRCClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(PandaRCClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 229, 23));
        PandaRCClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(PandaRCClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        PandaRCClass->setStatusBar(statusBar);

        retranslateUi(PandaRCClass);
        QObject::connect(btnBuild, SIGNAL(clicked()), PandaRCClass, SLOT(onBtnBuild()));
        QObject::connect(btnLogin, SIGNAL(clicked()), PandaRCClass, SLOT(onBtnLogin()));

        QMetaObject::connectSlotsByName(PandaRCClass);
    } // setupUi

    void retranslateUi(QMainWindow *PandaRCClass)
    {
        PandaRCClass->setWindowTitle(QApplication::translate("PandaRCClass", "PandaRC", nullptr));
        btnLogin->setText(QApplication::translate("PandaRCClass", "\347\231\273\345\275\225", nullptr));
        btnBuild->setText(QApplication::translate("PandaRCClass", "\345\273\272\347\253\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PandaRCClass: public Ui_PandaRCClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PANDARC_H
