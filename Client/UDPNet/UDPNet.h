#pragma once

#include <QObject>
#include <QUdpSocket>
#include "NetUtil/Packet.h"

#include "util/CommonHeader.h"
#include "log-writer/LogWriter.h"

class UDPNet : public QObject
{
	Q_OBJECT

public:
	UDPNet(QObject *parent);
	~UDPNet();

public:
	//���÷��͵�ַ�Ͷ˿�
	void SetSendAddress(QString& sendAddress, QString& sendPort) {
		m_sendAddress.setAddress(sendAddress);
		m_sendPort = sendPort;
	}

	//��������
	void SendPacket(Packet* pPacket) {
		qint64 res = m_sendSocket->writeDatagram((const char*)pPacket->GetData(), pPacket->GetDataSize(), m_sendAddress, m_sendPort.toInt());
		if (res == -1) {
			glog.debug(_T("send failure error:%s\n"), m_sendSocket->errorString().data());
		} else {
			glog.debug(_T("send successful size:%d\n"), pPacket->GetDataSize());
		}
		ReturnPacket(pPacket);
	}

	//���ý��յ�ַ�Ͷ˿�
	void SetSendAddress(QString& recvPort) {
		m_recvAddress = QHostAddress::AnyIPv4;
		m_recvPort = recvPort;
	}

	//��ʼ����������
	void InitRecvPacket() {
		m_recvSocket->bind(m_recvAddress, m_recvPort.toInt());
		connect(m_recvSocket, SIGNAL(readyRead()), this, SLOT(ReadPendingDatagrams()));
	}

	//��ȡ����
	void ReadPendingDatagrams() {
		while (m_recvSocket->hasPendingDatagrams()) {
			Packet* pPacket = ApplyPacket();
			qint64 avBytes = m_recvSocket->bytesAvailable();
			pPacket->Reserve(avBytes);
			QHostAddress peerAddress;
			quint16 peerPort;
			m_recvSocket->readDatagram((char*)pPacket->GetData(), avBytes, &peerAddress, &peerPort);
		}
	}

	//�������ݰ�
	Packet* ApplyPacket() {
		if (m_packetPool.size() > 0) {
			Packet* pPacket = m_packetPool.front();
			m_packetPool.pop();
			return pPacket;
		}
		Packet* pPacket = Packet::Create();
		return pPacket;
	}

	//�黹���ݰ�
	void ReturnPacket(Packet* pPacket) {
		pPacket->Reset();
		m_packetPool.push(pPacket);
	}


private:
	QUdpSocket* m_sendSocket;
	QHostAddress m_sendAddress;
	QString m_sendPort;
	std::queue<Packet*> m_sendQueue;

	QUdpSocket* m_recvSocket;
	QHostAddress m_recvAddress;
	QString m_recvPort;
	std::queue<Packet*> m_recvQueue;

	std::queue<Packet*> m_packetPool;
};
