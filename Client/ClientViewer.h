#pragma once

#include <QWidget>
#include "ui_ClientViewer.h"

class ClientViewer : public QWidget
{
	Q_OBJECT

public:
	ClientViewer(QWidget *parent = Q_NULLPTR);
	~ClientViewer();

public:
	void setUserID(int userID) { m_userID = userID;  }
	int getUserID() { return m_userID; }

protected:
	void paintEvent(QPaintEvent *event);
	void closeEvent(QCloseEvent *event);

private:
	Ui::ClientViewer ui;
	int m_userID;
};
