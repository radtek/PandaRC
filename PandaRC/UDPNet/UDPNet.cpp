#include "UDPNet.h"

UDPNet::UDPNet(QObject *parent)
	: QObject(parent)
{
	m_sendSocket = new QUdpSocket(); //�����׽���
	m_recvSocket = new QUdpSocket(); //�����׽���
}

UDPNet::~UDPNet()
{
	m_sendSocket->close();
	m_recvSocket->close();
	SAFE_DELETE(m_sendSocket);
	SAFE_DELETE(m_recvSocket);
}
