#ifndef __NETAPI_H__
#define __NETAPI_H__

#include "Common/Platform.h"

namespace NetAPI
{
	void StartupNetwork();
	HSOCKET CreateTcpSocket();
	HSOCKET CreateUdpSocket();

	void CloseSocket(HSOCKET nSock);
	bool ReuseAddr(HSOCKET nSock);
	bool NonBlock(HSOCKET nSock);
	bool NoDelay(HSOCKET nSock);
	bool Linger(HSOCKET nSock);

	bool Bind(HSOCKET nServerSock, uint32_t nIP, uint16_t nPort);
	bool Listen(HSOCKET nServerSock);
	HSOCKET Accept(HSOCKET nServerSock, uint32_t* pClientIP, uint16_t* nClientPort);
	bool Connect(HSOCKET nClientSock, const char* pServerIP, uint16_t nServerPort);

	int ReceiveBufSize(HSOCKET nSock);
	int SendBufSize(HSOCKET nSock);
	bool KeepAlive(HSOCKET nSock);

	uint32_t P2N(const char* pIP);
	const char* N2P(uint32_t nIP, char *pBuf, int nBufLen);
	bool GetPeerName(HSOCKET nSock, uint32_t* puPeerIP, uint16_t* puPeerPort);
	bool GetHostName(HSOCKET hSock, uint32_t* puHostIP, uint16_t* puHostPort);

	unsigned long long N2Hll(unsigned long long val);
	unsigned long long H2Nll(unsigned long long val);

	bool MyWSAIcotl(HSOCKET nSock);
};

#endif
