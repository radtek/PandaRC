#include "UDPNet.h"

UDPNet::UDPNet(QObject *parent)
	: QObject(parent)
{
	m_socket = new QUdpSocket();//´´½¨Ì×½Ó×Ö
}

UDPNet::~UDPNet()
{
}
