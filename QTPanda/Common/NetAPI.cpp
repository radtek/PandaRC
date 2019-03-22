#ifdef _WIN32
#include <WS2tcpip.h>
#include <MSWSock.h>
#endif
#include "NetAPI.h"

void NetAPI::StartupNetwork()
{
#ifdef __linux
	signal(SIGPIPE, SIG_IGN);
#else
	WSADATA wsa = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

HSOCKET NetAPI::CreateTcpSocket()
{
	HSOCKET hSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hSock == INVALID_SOCKET)
	{
#ifdef __linux
		printf("%s\n", strerror(errno));
#else
		printf("%s\n", Platform::LastErrorStr(GetLastError()));
#endif
	}
	return hSock;
}

HSOCKET NetAPI::CreateUdpSocket()
{
	HSOCKET hSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (hSock == INVALID_SOCKET)
	{
#ifdef __linux
		printf("%s\n", strerror(errno));
#else
		printf("%s", Platform::LastErrorStr(GetLastError()));
#endif
	}
	return hSock;
}

void NetAPI::CloseSocket(HSOCKET nSock)
{
#ifdef __linux
	close(nSock);
#else
	closesocket(nSock);
#endif

}

bool NetAPI::NonBlock(HSOCKET hSock)
{
#ifdef __linux
	int nFlags = fcntl(hSock, F_GETFL);
	if (nFlags == -1) 
	{
		printf("%s\n", strerror(errno));
		return false;
	}
	nFlags |= O_NONBLOCK;
	int nRet = fcntl(hSock, F_SETFL, nFlags);
	if (nRet == -1)
	{
		printf("%s\n", strerror(errno));
		return false;
	}
#else
	u_long nNonBlock = 1;
	int nRet = ioctlsocket(hSock, FIONBIO, &nNonBlock);
	if (nRet == SOCKET_ERROR)
	{
		printf("%s", Platform::LastErrorStr(GetLastError()));
		return false;
	}
#endif
	return true;
}

bool NetAPI::ReuseAddr(HSOCKET hSock)
{
	int nReuse = 1;
	int nRet = setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, (char*)&nReuse, sizeof(nReuse));
	if (nRet == SOCKET_ERROR)
	{
#ifdef __linux
		const char* psErr = strerror(errno);
		printf("%s\n", psErr);
#else
		const char* psErr = Platform::LastErrorStr(GetLastError());
		printf("%s", psErr);
#endif
		return false;
	}
	return true;
}

bool NetAPI::NoDelay(HSOCKET hSock)
{
	int nNoDelay = 1;
	int nRet = setsockopt(hSock, IPPROTO_TCP, TCP_NODELAY, (char*)&nNoDelay, sizeof(nNoDelay));
	if (nRet == SOCKET_ERROR)
	{
#ifdef __linux
		const char* psErr = strerror(errno);
		printf("%s\n", psErr);
#else
		const char* psErr = Platform::LastErrorStr(GetLastError());
		printf("%s", psErr);
#endif
		return false;
	}
	return true;
}

bool NetAPI::Linger(HSOCKET hSock)
{
	struct _LINGER
	{
		int l_onoff;
		int l_linger;
	} LINGER = { 1, 0 };
	int nRet = setsockopt(hSock, SOL_SOCKET, SO_LINGER, (char*)&LINGER, sizeof(LINGER));
	if (nRet == SOCKET_ERROR)
	{
#ifdef __linux
		const char* psErr = strerror(errno);
		printf("%s\n", psErr);
#else
		const char* psErr = Platform::LastErrorStr(GetLastError());
		printf("%s", psErr);
#endif
		return false;
	}
	return true;
}

bool NetAPI::Bind(HSOCKET hSock, uint32_t uIP, uint16_t nPort)
{
	struct sockaddr_in oAddr;
	oAddr.sin_family = AF_INET;
	oAddr.sin_addr.s_addr = uIP;
	oAddr.sin_port = htons(nPort);
	int nRet = ::bind(hSock, (struct sockaddr*)&oAddr, sizeof(oAddr));
	if (nRet == SOCKET_ERROR)
	{
#ifdef __linux
		const char* psErr = strerror(errno);
		printf("Bind port %d error (%s)\n", nPort, psErr);
#else
		const char* psErr = Platform::LastErrorStr(GetLastError());
		printf("Bind port %d error (%s)", nPort, psErr);
#endif
		return false;
	}
	return true;
}

bool NetAPI::Listen(HSOCKET hSock)
{
	int nRet = ::listen(hSock, SOMAXCONN);//SOMAXCONN=128Local
	if (nRet == SOCKET_ERROR)
	{
#ifdef __linux
		const char* psErr = strerror(errno);
		printf("%s\n", psErr);
#else
		const char* psErr = Platform::LastErrorStr(GetLastError());
		printf("%s", psErr);
#endif
		return false;
	}
	return true;
}

HSOCKET NetAPI::Accept(HSOCKET nServerSock, uint32_t* pClientIP, uint16_t* pClientPort)
{
	struct sockaddr_in oAddr;
	memset(&oAddr, 0, sizeof(oAddr));
#ifdef __linux
	socklen_t nAddrLen = sizeof(oAddr);
#else
	int nAddrLen = sizeof(oAddr);
#endif
	HSOCKET nClientSock = ::accept(nServerSock, (struct sockaddr*)&oAddr, &nAddrLen);
	if (nClientSock == SOCKET_ERROR)
	{
		return nClientSock;
	}
	if (pClientIP != NULL)
	{
		*pClientIP = oAddr.sin_addr.s_addr;
		//*pClientIP = ntohl(oAddr.sin_addr.s_addr);
	}
	if (pClientPort != NULL)
	{
		*pClientPort = ntohs(oAddr.sin_port);
	}
	return nClientSock;
}

bool NetAPI::Connect(HSOCKET nClientSock, const char* pszServerIP, uint16_t uServerPort)
{
	struct sockaddr_in oAddr;
	oAddr.sin_family = AF_INET;
	oAddr.sin_addr.s_addr = P2N(pszServerIP);
	oAddr.sin_port = htons(uServerPort);
	int nRet = connect(nClientSock, (struct sockaddr*)&oAddr, sizeof(oAddr));
	if (nRet == SOCKET_ERROR)
	{
#ifdef __linux
		const char* psErr = strerror(errno);
		printf("Connect %s %d fail: %s\n", pszServerIP, uServerPort, psErr);
#else
		const char* psErr = Platform::LastErrorStr(GetLastError());
		printf("Connect %s %d fail: %s", pszServerIP, uServerPort, psErr);
#endif
		return false;
	}
	return true;
}

int NetAPI::SendBufSize(HSOCKET hSock)
{
	int nOptVal = 0;
#ifdef __linux
	socklen_t nOptLen = sizeof(nOptVal); 
#else
	int nOptLen = sizeof(nOptVal);
#endif
	int nRet = getsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)&nOptVal, &nOptLen);
	if (nRet == SOCKET_ERROR)
	{
#ifdef __linux
		const char* psErr = strerror(errno);
		printf("%s\n", psErr);
#else
		const char* psErr = Platform::LastErrorStr(GetLastError());
		printf("%s", psErr);
#endif
		return 0;
	}
	return nOptVal;
}

int NetAPI::ReceiveBufSize(HSOCKET hSock)
{
	int nOptVal = 0;
#ifdef __linux
	socklen_t nOptLen = sizeof(nOptVal); 
#else
	int nOptLen = sizeof(nOptVal);
#endif
	int nRet = getsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char*)&nOptVal, &nOptLen);
	if (nRet == SOCKET_ERROR)
	{
#ifdef __linux
		const char* psErr = strerror(errno);
		printf("%s\n", psErr);
#else
		const char* psErr = Platform::LastErrorStr(GetLastError());
		printf("%s", psErr);
#endif
		return 0;
	}
	return nOptVal;
}

bool NetAPI::KeepAlive(HSOCKET hSock)
{
	/* 最好是用心跳，不用心跳，就只有用这个方法了，
	 * 没有其他的办法了，其实这个方法也是心跳，
	 * 要想不发送任何数据来检测对方online与否是不可能的！
	 * 该方法缺点是因为服务端主动发包，会占用额外的带宽和流量。
	 * */

	/* 单位：0.5秒 */
	/* 打开TCP KEEPALIVE开关*/
	int nKeepAlive = 1;
	/* 对一个连接进行有效性探测之前运行的最大非活跃时间间隔
	 * (没有任何数据交互)，默认值为14400(即2个小时)
	 * */
	int nKeepIdle = 3 * 60 * 2;
	/* 两个探测的时间间隔 */
	int nKeepIntvl = 3 * 2;
	/* 关闭一个非活跃连接之前进行探测的最大次数，默认为8次 */
	int nKeepCnt = 1;
	do
	{
#ifdef __linux
		if (setsockopt(hSock, SOL_SOCKET, SO_KEEPALIVE, (char*)&nKeepAlive, sizeof(nKeepAlive)) == -1)
			break;
		if (setsockopt(hSock, SOL_TCP, TCP_KEEPIDLE, (char*)&nKeepIdle, sizeof(nKeepIdle)) == -1)
			break;
		if (setsockopt(hSock, SOL_TCP, TCP_KEEPINTVL, (char*)&nKeepIntvl, sizeof(nKeepIntvl)) == -1)
			break;
		if (setsockopt(hSock, SOL_TCP, TCP_KEEPCNT, (char*)&nKeepCnt, sizeof(nKeepCnt)) == -1)
			break;
#endif
		return true;
	} while (0);
#ifdef __linux
	const char* psErr = strerror(errno);
	printf("%s\n", psErr);
#else
	const char* psErr = Platform::LastErrorStr(GetLastError());
	printf("%s", psErr);
#endif
	return false;
}

//uIP为网络字节顺序
const char* NetAPI::N2P(uint32_t uIP, char* pBuf, int nBufLen)
{
	const char* pStrIP = inet_ntop(AF_INET, &uIP, pBuf, nBufLen);
	if (pStrIP == NULL)
	{
		printf("%s\n", strerror(errno));
		return 0;
	}
	return pStrIP;
}

uint32_t NetAPI::P2N(const char* pszIP)
{
	struct in_addr oAddr;
	int nRet = inet_pton(AF_INET, pszIP, &oAddr);
	if (nRet <= 0)
	{
#ifdef __linux
		const char* pszErr = strerror(errno);
	    printf("%s\n", pszErr);
#else
		const char* pszErr = Platform::LastErrorStr(GetLastError());
	    printf("%s", pszErr);
#endif
		return 0;
	}
	//oAddr.s_addr为网络字节顺序
	return oAddr.s_addr;
}

bool NetAPI::GetPeerName(HSOCKET hSock, uint32_t* puPeerIP, uint16_t* puPeerPort)
{
	struct sockaddr_in oAddr;
#ifdef __linux
	socklen_t nAddrLen = sizeof(oAddr);
#else
	int nAddrLen = sizeof(oAddr);
#endif
	int nRet = getpeername(hSock, (sockaddr*)&oAddr, &nAddrLen);
	if (nRet == -1)
	{
#ifdef __linux
		const char* psErr = strerror(errno);
#else
		const char* psErr = Platform::LastErrorStr(GetLastError());
#endif
		printf("%s\n", psErr);
		return false;
	}
	if (puPeerIP != NULL)
	{
		*puPeerIP = oAddr.sin_addr.s_addr;	//网络字节顺序
	}
	if (puPeerPort != NULL)
	{
		*puPeerPort = ntohs(oAddr.sin_port);	//转成主机字节顺序
	}
	return true;
}

unsigned long long NetAPI::N2Hll(unsigned long long val)
{
#ifdef __linux
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
	{
		return (((unsigned long long)htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
	}
	else if (__BYTE_ORDER == __BIG_ENDIAN)
	{
		return val;
	}
#else
	return ntohll(val);
#endif
}

unsigned long long NetAPI::H2Nll(unsigned long long val)
{
#ifdef __linux
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
	{
		return (((unsigned long long)htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
	}
	else if (__BYTE_ORDER == __BIG_ENDIAN)
	{
		return val;
	}
#else
	return htonll(val);
#endif
}

bool NetAPI::MyWSAIcotl(HSOCKET nSock)
{
#ifdef _WIN32
	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = FALSE;
	DWORD status;

	// disable  new behavior using
	// IOCTL: SIO_UDP_CONNRESET
	status = WSAIoctl(nSock, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior), NULL, 0, &dwBytesReturned, NULL, NULL);
	if (SOCKET_ERROR == status)
	{
		DWORD dwErr = WSAGetLastError();
		if (WSAEWOULDBLOCK == dwErr)
		{
			return false;
		}
		printf("WSAIoctl(SIO_UDP_CONNRESET) Error: %d/n", dwErr);
		return false;
	}
#endif
	return true;
}
