// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <stdio.h>
#include "Hook.h"

char szOutputBuffer[1024] = { 0 };
NamedPipeClient goPipeClient;

///< 枚举窗口参数
typedef struct
{
	HWND hwndWindow; // 窗口句柄
	DWORD dwProcessID; // 进程ID
}EnumWindowsArg;

///< 枚举窗口回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	EnumWindowsArg *pArg = (EnumWindowsArg *)lParam;
	DWORD dwProcessID = 0;
	// 通过窗口句柄取得进程ID
	::GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (dwProcessID == pArg->dwProcessID)
	{
		pArg->hwndWindow = hwnd;
		// 找到了返回FALSE
		return FALSE;
	}
	// 没找到，继续找，返回TRUE
	return TRUE;
}
///< 通过进程ID获取窗口句柄
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
		sprintf(szOutputBuffer, "precess attach pid=%u", GetCurrentProcessId());
		MessageBox(NULL, szOutputBuffer, "信息", MB_ICONINFORMATION);
		if (!goPipeClient.Init())
		{
			MessageBox(NULL, "InitPipClient fail\n", "ERROR", MB_OK);
			return(false);
		}
		NSPROTO::CLT_REG reg;
		reg.processid = GetCurrentProcessId();
		if (!goPipeClient.SendMsg2Server(&reg))
		{
			MessageBox(NULL, "SendMsg2Server fail\n", "ERROR", MB_OK);
			return(false);
		}
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
		sprintf(szOutputBuffer, "precess detach pid=%u", GetCurrentProcessId());
		MessageBox(0, szOutputBuffer, "信息", MB_ICONINFORMATION);
	}
	break;
	}
	return TRUE;
}

