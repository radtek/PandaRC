#pragma once

#include <QObject>
#include <QUdpSocket>
#include <NetUtil\Packet.h>

class UDPNet : public QObject
{
	Q_OBJECT

public:
	UDPNet(QObject *parent);
	~UDPNet();

public:
	//设置发送地址和端口
	void SetSendAddress(QString& sendAddress, QString& sendPort) {
		m_sendAddress.setAddress(sendAddress);
		m_sendPort = sendPort;
	}

	//发送数据
	void SendPacket(Packet* pPacket) {
		//m_socket->writeDatagram();
	}


private:
	QUdpSocket* m_socket;
	QHostAddress m_sendAddress;
	QString m_sendPort;

};
