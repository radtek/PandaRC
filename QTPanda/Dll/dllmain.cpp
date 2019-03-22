// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "Hook.h"
char szOutputBuffer[1024] = { 0 };

///< ö�ٴ��ڲ���
typedef struct
{
	HWND hwndWindow; // ���ھ��
	DWORD dwProcessID; // ����ID
}EnumWindowsArg;

///< ö�ٴ��ڻص�����
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	EnumWindowsArg *pArg = (EnumWindowsArg *)lParam;
	DWORD dwProcessID = 0;
	// ͨ�����ھ��ȡ�ý���ID
	::GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (dwProcessID == pArg->dwProcessID)
	{
		pArg->hwndWindow = hwnd;
		// �ҵ��˷���FALSE
		return FALSE;
	}
	// û�ҵ��������ң�����TRUE
	return TRUE;
}
///< ͨ������ID��ȡ���ھ��
HWND GetWindowHwndByPID(DWORD dwProcessID)
{
	HWND hwndRet = NULL;
	EnumWindowsArg ewa;
	ewa.dwProcessID = dwProcessID;
	ewa.hwndWindow = NULL;
	EnumWindows(EnumWindowsProc, (LPARAM)&ewa);
	if (ewa.hwndWindow)
	{
		hwndRet = ewa.hwndWindow;
	}
	return hwndRet;
}

BOOL APIENTRY DllMain(HMODULE hInst,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		g_hinstDll = hInst;
		g_hForm = GetWindowHwndByPID(GetCurrentProcessId());
		sndmsg("I'm in*********************\n");
		//g_hForm = FindWindow(NULL, "QTPanda");
		sprintf(szOutputBuffer, "DLL�ѽ���Ŀ����̡�form:%u inst:%u", g_hForm, hInst);
		MessageBox(NULL, szOutputBuffer, "��Ϣ", MB_ICONINFORMATION);
		if (!InitHook())
		{
			MessageBox(NULL, "InitHook fail\n", "ERROR", MB_OK);
			return(false);
		}
	}
	break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
	{
		UninstallHook();
		sprintf(szOutputBuffer, "DLL�Ѵ�Ŀ�����ж�� proc id:%d", GetCurrentProcessId());
		MessageBox(NULL, szOutputBuffer, "��Ϣ", MB_ICONINFORMATION);
	}
	break;
	}
	return TRUE;
}

