// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		MessageBox(NULL, "DLL�ѽ���Ŀ����̡�", "��Ϣ", MB_ICONINFORMATION);
	}
	break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
	{
		MessageBox(NULL, "DLL�Ѵ�Ŀ�����ж�ء�", "��Ϣ", MB_ICONINFORMATION);
	}
	break;
	}
	return TRUE;
}

