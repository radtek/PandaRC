#include "UDPNet.h"

UDPNet::UDPNet(QObject *parent)
	: QObject(parent)
{
	m_socket = new QUdpSocket();//�����׽���
}

UDPNet::~UDPNet()
{
}
