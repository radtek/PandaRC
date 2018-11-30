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
	void SetKey(const std::string& key) { m_key = key;  }

protected:
	void closeEvent(QCloseEvent *event);

private:
	Ui::ClientViewer ui;

	std::string m_key;
};
