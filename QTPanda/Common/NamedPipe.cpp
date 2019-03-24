#include "NamedPipe.h"
#include "Common/XThread.h"
#include <Windows.h>

const int nPipeBufSize = 1024 * 64;
static char szOutputBuf[nPipeBufSize] = {0};
const char* pszPipeName = "\\\\.\\pipe\\my-comunicate";

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

int WaitFinish(HANDLE hPipe, OVERLAPPED& tagOver, DWORD& dwTransBytes)
{
	bool bPendingIO = false;
	switch (GetLastError())
	{
	//正在连接中
	case ERROR_IO_PENDING:
		bPendingIO = true;
		break;
	//已经连接
	case ERROR_PIPE_CONNECTED:
		SetEvent(tagOver.hEvent);
		break;
	}

	DWORD dwWait = -1;
	//等待读写操作完成
	dwWait = WaitForSingleObject(tagOver.hEvent, INFINITE);
	switch (dwWait)
	{
	case 0:
		if (bPendingIO)
		{
			//获取Overlapped结果
			if (GetOverlappedResult(hPipe, &tagOver, &dwTransBytes, FALSE) == FALSE)
			{
				sprintf(szOutputBuf, "GetOverlappedResult failed GLE=%d\n", GetLastError());
				OutputDebugString(szOutputBuf);
				return -1;
			}
		}
		break;

	//	读写完成
	case WAIT_IO_COMPLETION:
		break;

	}
	return 0;
}

NamedPipeServer::NamedPipeServer()
{
	m_pAcceptThread = NULL;
	m_hWnd = NULL;
}

bool NamedPipeServer::Init(HWND hWnd)
{
	m_hWnd = hWnd;
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
			pszPipeName,									// pipe name 
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,		// read/write access 
			PIPE_TYPE_MESSAGE |			// message type pipe 
			PIPE_READMODE_MESSAGE |		// message-read mode 
			PIPE_WAIT,                  // blocking mode 
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

		OVERLAPPED tagOver;
		memset(&tagOver, 0, sizeof(tagOver));
		bool bSuccess = ConnectNamedPipe(hPipe, NULL) ?  TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (!bSuccess)
		{
			CloseHandle(hPipe);
			sprintf(szOutputBuf, "ConnectNamedPipe failed! GLE=%d\n", GetLastError());
			OutputDebugString(szOutputBuf);
			continue;
		}

		DWORD dwTransBytes = 0;
		if (WaitFinish(hPipe, tagOver, dwTransBytes) != 0)
		{
			CloseHandle(hPipe);
			continue;
		}

		pThread = new XThread();
		PIPE_NODE* pAcceptNode = new PIPE_NODE();
		pAcceptNode->hPipe = hPipe;
		pAcceptNode->pThread = pThread;
		pAcceptNode->pServer = pServer;

		bSuccess = pThread->Create(&NamedPipeServer::ServerReadThread, pAcceptNode);
		if (!bSuccess)
		{
			CloseHandle(hPipe);
			delete pThread;
			return;
		}
	}
}

void NamedPipeServer::ServerReadThread(void* pParam)
{
	PIPE_NODE* pAcceptNode = (PIPE_NODE*)pParam;
	HANDLE hPipe = pAcceptNode->hPipe;
	DWORD dwBindProcessID = 0;

	char szReadBuf[nPipeBufSize] = { 0 };
	DWORD dwReadSize = 0;

	OVERLAPPED tagOver;
	memset(&tagOver, 0x0, sizeof(tagOver));
	tagOver.hEvent = CreateEvent(NULL,//默认属性
		TRUE, //手工reset
		TRUE, //初始状态signaled
		NULL); //未命名

	while (1)
	{
		//读
		int nRet = ReadFile(hPipe, &szReadBuf, sizeof(szReadBuf), &dwReadSize, &tagOver);
		bool bRet = nRet != 0 ? true : (GetLastError() == ERROR_IO_PENDING);
		if (!bRet)
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
		if (WaitFinish(hPipe, tagOver, dwReadSize) != 0)
		{
			break;
		}
		NSPROTO::HEAD* pHeader = (NSPROTO::HEAD*)szReadBuf;
		if (pHeader->cmd == NSCMD::eCLT_REG)
		{
			NSPROTO::CLT_REG reg = *(NSPROTO::CLT_REG*)szReadBuf;
			pAcceptNode->pServer->AddProcessPipe(reg.processid, hPipe);
			dwBindProcessID = reg.processid;
			sprintf(szOutputBuf, "Server recv client reg pid=%d pipe=%lld thread=%u\n", reg.processid, (DWORD_PTR)hPipe, GetCurrentThreadId());
			OutputDebugString(szOutputBuf);
		}
		else if (pHeader->cmd == NSCMD::eCLT_UNREG)
		{
			NSPROTO::CLT_UNREG unreg = *(NSPROTO::CLT_UNREG*)szReadBuf;
			pAcceptNode->pServer->RemoveProcessPipe(unreg.processid);
			sprintf(szOutputBuf, "Server recv client unreg pid=%d pipe=%lld thread=%u\n", unreg.processid, (DWORD_PTR)hPipe, GetCurrentThreadId());
			OutputDebugString(szOutputBuf);
			break;
		}
		else if (pHeader->cmd == NSCMD::eCOM_ERR)
		{
			NSPROTO::COM_ERR err = *(NSPROTO::COM_ERR*)szReadBuf;
			sprintf(szOutputBuf, "Server recv client error pid=%d pipe=%lld err=%s thread=%u\n", err.processid, (DWORD_PTR)hPipe, err.errormsg, GetCurrentThreadId());
			OutputDebugString(szOutputBuf);
		}
		else
		{
			NSPROTO::COM_DATA data = *(NSPROTO::COM_DATA*)szReadBuf;
			pAcceptNode->pServer->GetMailBox().Send(data);
		}
		memset(szReadBuf, 0x0, sizeof(szReadBuf));
	}
	pAcceptNode->pServer->RemovePipeProcess(hPipe);
	CloseHandle(hPipe);
	delete pAcceptNode;
	sprintf(szOutputBuf, "Server read thread exit pid=%d\n", dwBindProcessID);
	OutputDebugString(szOutputBuf);
}

bool NamedPipeServer::SendMsg2Client(DWORD dwProcessID, NSPROTO::PROTO* data)
{
	HANDLE hPipe = GetProcessPipe(dwProcessID);
	if (hPipe == NULL)
	{
		sprintf(szOutputBuf, "SendMsg2Client can not find pipe by process. pid=%d\n", dwProcessID);
		OutputDebugString(szOutputBuf);
		return false;
	}
	DWORD dwReadSize = 0;
	OVERLAPPED tagOver;
	memset(&tagOver, 0x0, sizeof(tagOver));

	tagOver.hEvent = CreateEvent(NULL,//默认属性
		TRUE,	//手工reset
		TRUE,	//初始状态signaled
		NULL);	//未命名

	DWORD dwHadWrite = 0;
	int nRet = WriteFile(hPipe, data, data->size, &dwHadWrite, &tagOver);
	if (nRet == 0)
	{
		sprintf(szOutputBuf, "WriteFile to pipe failed. GLE=%d\n", GetLastError());
		OutputDebugString(szOutputBuf);
		return false;
	}

	if (WaitFinish(hPipe, tagOver, dwHadWrite) != 0)
	{
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
			FILE_FLAG_OVERLAPPED,	// default attributes 
			NULL);					// no template file 


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
			OutputDebugString("Could not open pipe: 20 second wait timed out.");
			return false;
		}
	}

	// The pipe connected; change to message-read mode. 
	//DWORD dwMode = PIPE_READMODE_MESSAGE;
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	int nRet = SetNamedPipeHandleState(
		m_hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 

	if (nRet == 0)
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

	bool bSuccess = pThread->Create(&NamedPipeClient::ClientReadThread, pClientNode);
	if (!bSuccess)
	{
		delete pThread;
		return false;
	}
	return true;
}

bool NamedPipeClient::SendMsg2Server(NSPROTO::PROTO* data)
{
	if (IsShutDown())
	{
		return false;
	}
	OVERLAPPED tagOver;
	memset(&tagOver, 0x0, sizeof(tagOver));

	tagOver.hEvent = CreateEvent(NULL,//默认属性
		TRUE,//手工reset
		TRUE,//初始状态signaled
		NULL);//未命名

	DWORD dwHadWrite = 0;
	int nRet = WriteFile(m_hPipe, data, data->size, &dwHadWrite, &tagOver);
	if (nRet == 0)
	{
		sprintf(szOutputBuf, "WriteFile to pipe failed. GLE=%d\n", GetLastError());
		OutputDebugString(szOutputBuf);
		return false;
	}
	if (WaitFinish(m_hPipe, tagOver, dwHadWrite) != 0)
	{
		return false;
	}
	return true;
}

void NamedPipeClient::ClientReadThread(void* pParam)
{
	PIPE_NODE* pClientNode = (PIPE_NODE*)pParam;
	HANDLE hPipe = pClientNode->hPipe;

	char szReadBuf[nPipeBufSize] = { 0 };
	DWORD dwReadSize = 0;

	OVERLAPPED tagOver;
	memset(&tagOver, 0x0, sizeof(tagOver));
	tagOver.hEvent = CreateEvent(NULL,//默认属性
		TRUE, //手工reset
		TRUE, //初始状态signaled
		NULL); //未命名

	while (!pClientNode->pClient->IsShutDown())
	{
		//读
		int nRet = ReadFile(hPipe, &szReadBuf, sizeof(szReadBuf), &dwReadSize, &tagOver);
		bool bRet = nRet != 0 ? true : (GetLastError() == ERROR_IO_PENDING);
		if (!bRet)
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
		if (WaitFinish(hPipe, tagOver, dwReadSize) != 0)
		{
			break;
		}
		NSPROTO::HEAD* pHeader = (NSPROTO::HEAD*)szReadBuf;
		if (pHeader->cmd == NSCMD::eSRV_EXITTHREAD)
		{
			pClientNode->pClient->ShutDown();
		}
		else
		{
			NSPROTO::COM_DATA data = *(NSPROTO::COM_DATA*)szReadBuf;
			sprintf(szOutputBuf, "Client recv data len=%d pid=%d\n", data.len, GetCurrentProcessId());
			OutputDebugString(szOutputBuf);
		}
		memset(szReadBuf, 0x0, sizeof(szReadBuf));;
	}
	CloseHandle(hPipe);
	delete pClientNode;
}
