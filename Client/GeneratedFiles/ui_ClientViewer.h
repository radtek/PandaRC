/********************************************************************************
** Form generated from reading UI file 'ClientViewer.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTVIEWER_H
#define UI_CLIENTVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientViewer
{
public:

    void setupUi(QWidget *ClientViewer)
    {
        if (ClientViewer->objectName().isEmpty())
            ClientViewer->setObjectName(QStringLiteral("ClientViewer"));
        ClientViewer->resize(400, 300);

        retranslateUi(ClientViewer);

        QMetaObject::connectSlotsByName(ClientViewer);
    } // setupUi

    void retranslateUi(QWidget *ClientViewer)
    {
        ClientViewer->setWindowTitle(QApplication::translate("ClientViewer", "ClientViewer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientViewer: public Ui_ClientViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTVIEWER_H
