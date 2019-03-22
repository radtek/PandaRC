#include "NamedPipe.h"
#include "Common/XThread.h"
#include <Windows.h>

const int nPipeBufSize = 1024 * 64;
static char szOutputBuf[1024] = {0};
const char* pszPipeName = "\\\\.\\pipe\\comunicate";

struct PIPE_NODE
{
	HANDLE hPipe;
	XThread* pThread;
	NamedPipeServer* pServer;
	NamedPipeClient* pClient;
	PIPE_NODE()
	{
		hPipe = 0;
		pThread = NULL;
		pServer = NULL;
		pClient = NULL;
	}
	~PIPE_NODE()
	{
		delete pThread;
	}
};

NamedPipeServer::NamedPipeServer()
{
	m_pAcceptThread = NULL;
}

bool NamedPipeServer::Init()
{
	m_pAcceptThread = new XThread();
	return m_pAcceptThread->Create(&NamedPipeServer::AcceptThread, this);
}

void NamedPipeServer::AcceptThread(void* pParam)
{
	NamedPipeServer* pServer = (NamedPipeServer*)pParam;

	HANDLE hPipe = 0;
	XThread* pThread = 0;
	
	for (;;)
	{
		hPipe = CreateNamedPipe(
			pszPipeName,             // pipe name 
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			nPipeBufSize,             // output buffer size 
			nPipeBufSize,             // input buffer size 
			0,                        // client time-out 
			NULL);                    // default security attribute 

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			sprintf(szOutputBuf, "CreateNamedPipe failed, GLE=%d.\n", GetLastError());
			OutputDebugString(szOutputBuf);
			return;
		}
		bool bConnected = ConnectNamedPipe(hPipe, NULL) ?  TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (bConnected)
		{
			pThread = new XThread();
			PIPE_NODE* pAcceptNode = new PIPE_NODE();
			pAcceptNode->hPipe = hPipe;
			pAcceptNode->pThread = pThread;
			pAcceptNode->pServer = pServer;

			bool bSuccess = pThread->Create(&NamedPipeServer::ServerReadThread, pAcceptNode);
			if (!bSuccess)
			{
				delete pThread;
				return;
			}
		}
		else
		{
			// The client could not connect, so close the pipe. 
			CloseHandle(hPipe);
		}
	}
}

void NamedPipeServer::ServerReadThread(void* pParam)
{
	PIPE_NODE* pAcceptNode = (PIPE_NODE*)pParam;
	HANDLE hHeap = GetProcessHeap();

	char* pchRequest = (char*)HeapAlloc(hHeap, 0, nPipeBufSize * sizeof(char));
	char* pchRequestPos = pchRequest;

	char* pchReply = (char*)HeapAlloc(hHeap, 0, nPipeBufSize * sizeof(char));

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL bSuccess = FALSE;

// The thread's parameter is a handle to a pipe object instance. 
	HANDLE hPipe = pAcceptNode->hPipe;

	// Do some extra error checking since the app will keep running even if this
	// thread fails.
	if (pParam == NULL)
	{
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		OutputDebugString("ERROR - Pipe Server Failure: no accept node\n");
		return;
	}

	if (pchRequest == NULL)
	{
		OutputDebugString("ERROR - Pipe Server Failure: InstanceThread got an unexpected NULL heap allocation.\n");
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		return;
	}

	if (pchReply == NULL)
	{
		OutputDebugString("ERROR - Pipe Server Failure:\n   InstanceThread got an unexpected NULL heap allocation.\n");
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return;
	}

	// Loop until done reading
	for (;;)
	{
		// Read client requests from the pipe. This simplistic code only allows messages
		// up to BUFSIZE characters in length.
		bSuccess = ReadFile(
			hPipe,									// handle to pipe 
			pchRequestPos,							// buffer to receive data 
			pchRequest+nPipeBufSize-pchRequestPos,	// size of buffer 
			&cbBytesRead,							// number of bytes read 
			NULL);									// not overlapped I/O 

		if (!bSuccess || cbBytesRead == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				OutputDebugString("Server InstanceThread: client disconnected.\n");
			}
			else
			{
				sprintf(szOutputBuf, "Server InstanceThread ReadFile failed, GLE=%d.\n", GetLastError());
				OutputDebugString(szOutputBuf);
			}
			break;
		}

		if (pchRequestPos - pchRequest <= 2)
		{
			continue;
		}
		int nPacketSize = *(uint16_t*)pchRequest;
		if (nPacketSize >= nPipeBufSize)
		{
			sprintf(szOutputBuf, "Server ReadFile packet invalid size: %d.\n", nPacketSize);
			OutputDebugString(szOutputBuf);
			break;
		}
		if (pchRequestPos - pchRequest < nPacketSize)
		{
			continue;
		}

		NSPROTO::DATA data = *(NSPROTO::DATA*)pchRequest;
		int nCurrDataSize = pchRequestPos - pchRequest;
		if (nCurrDataSize - nPacketSize)
		{
			memmove(pchRequest, pchRequestPos, nCurrDataSize-nPacketSize);
			pchRequestPos = pchRequest + (nCurrDataSize-nPacketSize);
		}
		sprintf(szOutputBuf, "Server recv size:%d", data.size);
		OutputDebugString(szOutputBuf);
	}

	// Flush the pipe to allow the client to read the pipe's contents 
	// before disconnecting. Then disconnect the pipe, and close the 
	// handle to this pipe instance. 

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);
	delete pAcceptNode;
}

bool NamedPipeServer::SendMsg2Client(HANDLE hPipe, NSPROTO::DATA* data)
{
	// Write the reply to the pipe. 
	DWORD cbWritten = 0;
	bool bSuccess = WriteFile(
		hPipe,      // handle to pipe 
		data,		// buffer to write from 
		data->size,	// number of bytes to write 
		&cbWritten, // number of bytes written 
		NULL);      // not overlapped I/O 

	if (!bSuccess || data->size != cbWritten)
	{
		sprintf(szOutputBuf, "InstanceThread WriteFile failed, GLE=%d.\n", GetLastError());
		OutputDebugString(szOutputBuf);
		return false;
	}
	return true;
}




/////////////////////////////////////CLIENT
NamedPipeClient::NamedPipeClient()
{
	m_hPipe = 0;
	m_bShutDown = false;
}

bool NamedPipeClient::Init()
{
	while (1)
	{
		m_hPipe = CreateFile(
			pszPipeName,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

							// Break if the pipe handle is valid. 

		if (m_hPipe != INVALID_HANDLE_VALUE)
		{
			break;
		}

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			sprintf(szOutputBuf, "Could not open pipe. GLE=%d\n", GetLastError());
			OutputDebugString(szOutputBuf);
			return false;
		}

		// All pipe instances are busy, so wait for 20 seconds. 
		if (!WaitNamedPipe(pszPipeName, 20000))
		{
			sprintf(szOutputBuf, "Could not open pipe: 20 second wait timed out.", GetLastError());
			OutputDebugString(szOutputBuf);
			return false;
		}
	}

	// The pipe connected; change to message-read mode. 
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	bool bSuccess = SetNamedPipeHandleState(
		m_hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 

	if (!bSuccess)
	{
		sprintf(szOutputBuf, "SetNamedPipeHandleState failed. GLE=%d\n", GetLastError());
		OutputDebugString(szOutputBuf);
		return false;
	}

	XThread* pThread = new XThread();
	PIPE_NODE* pClientNode = new PIPE_NODE();
	pClientNode->hPipe = m_hPipe;
	pClientNode->pThread = pThread;
	pClientNode->pClient = this;

	bSuccess = pThread->Create(&NamedPipeClient::ClientReadThread, pClientNode);
	if (!bSuccess)
	{
		delete pThread;
		return false;
	}
	return true;
}

bool NamedPipeClient::SendMsg2Server(NSPROTO::DATA* data)
{
	DWORD cbWritten = 0;
	bool bSuccess = WriteFile(
		m_hPipe,                // pipe handle 
		data,					// message 
		data->size,             // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!bSuccess)
	{
		sprintf(szOutputBuf, "WriteFile to pipe failed. GLE=%d\n", GetLastError());
		OutputDebugString(szOutputBuf);
		return false;
	}
	return true;
}

void NamedPipeClient::ClientReadThread(void* pParam)
{
	PIPE_NODE* pClientNode = (PIPE_NODE*)pParam;

	HANDLE hHeap = GetProcessHeap();
	char* pchRequest = (char*)HeapAlloc(hHeap, 0, nPipeBufSize * sizeof(char));
	char* pchRequestPos = pchRequest;

	while (pClientNode->pClient->m_bShutDown)
	{
		// Read from the pipe. 
		DWORD cbBytesRead = 0;
		bool bSuccess = ReadFile(
			pClientNode->hPipe,							// handle to pipe 
			pchRequestPos,								// buffer to receive data 
			pchRequest + nPipeBufSize - pchRequestPos,	// size of buffer 
			&cbBytesRead,								// number of bytes read 
			NULL);										// not overlapped I/O 

		if (!bSuccess || cbBytesRead == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				OutputDebugString("Client InstanceThread: client disconnected.\n");
			}
			else
			{
				sprintf(szOutputBuf, "Client InstanceThread ReadFile failed, GLE=%d.\n", GetLastError());
				OutputDebugString(szOutputBuf);
			}
			break;
		}

		if (pchRequestPos - pchRequest <= 2)
		{
			continue;
		}
		int nPacketSize = *(uint16_t*)pchRequest;
		if (nPacketSize >= nPipeBufSize)
		{
			sprintf(szOutputBuf, "Client ReadFile packet invalid size: %d.\n", nPacketSize);
			OutputDebugString(szOutputBuf);
			break;
		}
		if (pchRequestPos - pchRequest < nPacketSize)
		{
			continue;
		}

		NSPROTO::DATA data = *(NSPROTO::DATA*)pchRequest;
		int nCurrDataSize = pchRequestPos - pchRequest;
		if (nCurrDataSize - nPacketSize)
		{
			memmove(pchRequest, pchRequestPos, nCurrDataSize - nPacketSize);
			pchRequestPos = pchRequest + (nCurrDataSize - nPacketSize);
		}

		sprintf(szOutputBuf, "Client recv size:%d", data.size);
		OutputDebugString(szOutputBuf);
	}

	CloseHandle(pClientNode->hPipe);
	HeapFree(hHeap, 0, pchRequest);
	delete pClientNode;
}
