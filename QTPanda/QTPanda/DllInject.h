#pragma once
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <TlHelp32.h>

char szOutputBuf[1024] = { 0 };
DWORD FindProcess(LPCTSTR lpszProcess)
{
	DWORD dwRet = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe32);
	do
	{
		if (lstrcmpi(pe32.szExeFile, lpszProcess) == 0)
		{
			dwRet = pe32.th32ProcessID;
			//char szName[32] = { 0 };
			//sprintf_s(szName, "found------>%s\n", pe32.szExeFile);
			//OutputDebugStringA(szName);
			break;
		}
		else
		{
			//char szName[32] = { 0 };
			//sprintf_s(szName, "%s\n", pe32.szExeFile);
			//OutputDebugStringA(szName);
		}
	} while (Process32Next(hSnapshot, &pe32));
	CloseHandle(hSnapshot);
	return dwRet;
}

bool FindProcessModule(LPCTSTR lpszProcess, LPCTSTR lpszModule, MODULEENTRY32* out_pMe)
{
	DWORD dwProcessID = FindProcess(lpszProcess);
	if (dwProcessID == 0)
	{
		OutputDebugStringA("�Ҳ�������\n");
		return false;
	}
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);
	if (hSnapshot == NULL)
	{
		OutputDebugStringA("�������̿���ʧ��\n");
		return false;
	}

	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hSnapshot, &me32))
	{
		OutputDebugStringA("ȡ����ģ��ʧ��\n");
		return false;
	}
	do
	{
		if (lstrcmpi(me32.szModule, lpszModule) == 0)
		{
			*out_pMe = me32;
			return true;
		}
	} while (Module32Next(hSnapshot, &me32));
	CloseHandle(hSnapshot);
	return false;
}

bool WriteString2Process(HANDLE hProcess, LPCTSTR lpszName, LPVOID& Out_lpBuf, SIZE_T& Out_dwSize)
{
	// ��Ŀ����̵�ַ�ռ�д������  
	SIZE_T dwSize, dwWritten;
	dwSize = lstrlen(lpszName) + 1;
	LPVOID lpBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (lpBuf == NULL)
	{
		OutputDebugStringA("����Զ���ڴ�ʧ��\n");
		return false;
	}
	if (WriteProcessMemory(hProcess, lpBuf, (LPVOID)lpszName, dwSize, &dwWritten))
	{
		// Ҫд���ֽ�����ʵ��д���ֽ�������ȣ�����ʧ��  
		if (dwWritten != dwSize)
		{
			VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
			OutputDebugStringA("д��Զ���ڴ����\n");
			return false;
		}
	}
	else
	{
		OutputDebugStringA("д��Զ���ڴ�ʧ��\n");
		return false;
	}
	Out_lpBuf = lpBuf;
	Out_dwSize = dwSize;
	return true;
}

bool WriteDword2Process(HANDLE hProcess, DWORD dwValue, LPVOID& Out_lpBuf, SIZE_T& Out_dwSize)
{
	// ��Ŀ����̵�ַ�ռ�д������  
	SIZE_T dwSize, dwWritten;
	dwSize = sizeof(dwValue);
	LPVOID lpBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (lpBuf == NULL)
	{
		OutputDebugStringA("����Զ���ڴ�ʧ��\n");
		return false;
	}
	if (WriteProcessMemory(hProcess, lpBuf, (LPVOID)&dwValue, dwSize, &dwWritten))
	{
		// Ҫд���ֽ�����ʵ��д���ֽ�������ȣ�����ʧ��  
		if (dwWritten != dwSize)
		{
			VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
			OutputDebugStringA("д��Զ���ڴ����\n");
			return false;
		}
	}
	else
	{
		OutputDebugStringA("д��Զ���ڴ�ʧ��\n");
		return false;
	}
	Out_lpBuf = lpBuf;
	Out_dwSize = dwSize;
	return true;
}

DWORD GetRemoteLastError(HANDLE hProcess)
{
	LPVOID pFunc = GetLastError;
	DWORD dwID = 0;
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, (LPVOID)0, 0, &dwID);
	WaitForSingleObject(hThread, INFINITE);
	DWORD dwError = 0;
	GetExitCodeThread(hThread, &dwError);
	CloseHandle(hThread);
	sprintf_s(szOutputBuf, "Last Error:%d\n", dwError);
	OutputDebugStringA(szOutputBuf);
	return dwError;
}

bool InjectDll(LPCTSTR lpszProcess, LPCTSTR lpszDllName)
{
	DWORD dwProcessID = FindProcess(lpszProcess);
	if (dwProcessID == 0)
	{
		OutputDebugStringA("�Ҳ�������\n");
		return false;
	}
	// ��Ŀ�����  
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessID);
	if (hProcess == NULL)
	{
		OutputDebugStringA("�򿪽���ʧ��\n");
		return false;
	}

	// ��Ŀ����̵�ַ�ռ�д������  
	SIZE_T dwSize;
	LPVOID lpBuf;
	if (!WriteString2Process(hProcess, lpszDllName, lpBuf, dwSize))
	{
		CloseHandle(hProcess);
		return false;
	}

	// ʹĿ����̵���LoadLibrary������DLL  
	DWORD dwID;
	//Win32��ÿ��Ӧ�ó��򶼻��kernel32.dll���ص����̵�ַ�ռ���һ���̶��ĵ�ַ����������ĺ���LoadLibaryA��ַ��Ŀ�������Ҳ����Ч�ġ�
	LPVOID pFunc = LoadLibraryA;
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, lpBuf, 0, &dwID);
	if (hThread == NULL)
	{
		CloseHandle(hProcess);

		sprintf_s(szOutputBuf, "����Զ���߳�ʧ��:%d\n", GetLastError());
		OutputDebugStringA(szOutputBuf);
		return false;
	}
	// �ȴ�LoadLibrary�������  
	WaitForSingleObject(hThread, INFINITE);
	DWORD dwExitCode = 0;
	GetExitCodeThread(hThread, &dwExitCode);
	// �ͷ�Ŀ�����������Ŀռ�  
	VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return true;
}

bool UnjectDll(LPCTSTR lpszProcess, LPCTSTR lpszDllName)
{
	DWORD dwProcessID = FindProcess(lpszProcess);
	if (dwProcessID == 0)
	{
		OutputDebugStringA("�Ҳ�������\n");
		return false;
	}
	// ��Ŀ�����  
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessID);
	if (hProcess == NULL)
	{
		OutputDebugStringA("�򿪽���ʧ��\n");
		return false;
	}

	// ʹĿ����̵���FreeLibrary��ж��DLL  
	LPVOID pFunc = FreeLibrary;
	MODULEENTRY32 me;
	if (!FindProcessModule(lpszProcess, lpszDllName, &me))
	{
		CloseHandle(hProcess);

		sprintf_s(szOutputBuf, "��ȡģ����ʧ��: %s\n", lpszDllName);
		OutputDebugStringA(szOutputBuf);
		return false;
	}
	DWORD dwID, dwCode;
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, me.modBaseAddr, 0, &dwID);
	if (hThread == NULL)
	{
		CloseHandle(hProcess);

		sprintf_s(szOutputBuf, "����Զ���߳�ʧ��:%d\n", GetLastError());
		OutputDebugStringA(szOutputBuf);
		return false;
	}
	// �ȴ�FreeLibraryж�����  
	WaitForSingleObject(hThread, INFINITE);
	// ���FreeLibrary�ķ���ֵ  
	GetExitCodeThread(hThread, &dwCode);
	if (dwCode == 0)
	{
		CloseHandle(hThread);
		DWORD dwError = GetRemoteLastError(hProcess);
		CloseHandle(hProcess);

		sprintf_s(szOutputBuf, "ж��DLLʧ�� Err Code: %d\n", dwError);
		OutputDebugStringA(szOutputBuf);
		return false;
	}
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return true;
}