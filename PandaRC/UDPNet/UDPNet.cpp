#include "UDPNet.h"

UDPNet::UDPNet(QObject *parent)
	: QObject(parent)
{
	m_sendSocket = new QUdpSocket(); //创建套接字
	m_recvSocket = new QUdpSocket(); //创建套接字
}

UDPNet::~UDPNet()
{
	m_sendSocket->close();
	m_recvSocket->close();
	SAFE_DELETE(m_sendSocket);
	SAFE_DELETE(m_recvSocket);
}
