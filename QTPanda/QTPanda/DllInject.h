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
		OutputDebugStringA("找不到进程\n");
		return false;
	}
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);
	if (hSnapshot == NULL)
	{
		OutputDebugStringA("创建进程快照失败\n");
		return false;
	}

	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hSnapshot, &me32))
	{
		OutputDebugStringA("取进程模块失败\n");
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
	// 向目标进程地址空间写入名称  
	SIZE_T dwSize, dwWritten;
	dwSize = lstrlen(lpszName) + 1;
	LPVOID lpBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (lpBuf == NULL)
	{
		OutputDebugStringA("分配远程内存失败\n");
		return false;
	}
	if (WriteProcessMemory(hProcess, lpBuf, (LPVOID)lpszName, dwSize, &dwWritten))
	{
		// 要写入字节数与实际写入字节数不相等，仍属失败  
		if (dwWritten != dwSize)
		{
			VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
			OutputDebugStringA("写入远程内存错误\n");
			return false;
		}
	}
	else
	{
		OutputDebugStringA("写入远程内存失败\n");
		return false;
	}
	Out_lpBuf = lpBuf;
	Out_dwSize = dwSize;
	return true;
}

bool WriteDword2Process(HANDLE hProcess, DWORD dwValue, LPVOID& Out_lpBuf, SIZE_T& Out_dwSize)
{
	// 向目标进程地址空间写入名称  
	SIZE_T dwSize, dwWritten;
	dwSize = sizeof(dwValue);
	LPVOID lpBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (lpBuf == NULL)
	{
		OutputDebugStringA("分配远程内存失败\n");
		return false;
	}
	if (WriteProcessMemory(hProcess, lpBuf, (LPVOID)&dwValue, dwSize, &dwWritten))
	{
		// 要写入字节数与实际写入字节数不相等，仍属失败  
		if (dwWritten != dwSize)
		{
			VirtualFreeEx(hProcess, lpBuf, dwSize, MEM_DECOMMIT);
			OutputDebugStringA("写入远程内存错误\n");
			return false;
		}
	}
	else
	{
		OutputDebugStringA("写入远程内存失败\n");
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
		OutputDebugStringA("找不到进程\n");
		return false;
	}
	// 打开目标进程  
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessID);
	if (hProcess == NULL)
	{
		OutputDebugStringA("打开进程失败\n");
		return false;
	}

	// 向目标进程地址空间写入名称  
	SIZE_T dwSize;
	LPVOID lpBuf;
	if (!WriteString2Process(hProcess, lpszDllName, lpBuf, dwSize))
	{
		CloseHandle(hProcess);
		return false;
	}

	// 使目标进程调用LoadLibrary，加载DLL  
	DWORD dwID;
	//Win32下每个应用程序都会把kernel32.dll加载到进程地址空间中一个固定的地址，所以这里的函数LoadLibaryA地址在目标进程中也是有效的。
	LPVOID pFunc = LoadLibraryA;
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, lpBuf, 0, &dwID);
	if (hThread == NULL)
	{
		CloseHandle(hProcess);

		sprintf_s(szOutputBuf, "创建远程线程失败:%d\n", GetLastError());
		OutputDebugStringA(szOutputBuf);
		return false;
	}
	// 等待LoadLibrary加载完毕  
	WaitForSingleObject(hThread, INFINITE);
	DWORD dwExitCode = 0;
	GetExitCodeThread(hThread, &dwExitCode);
	// 释放目标进程中申请的空间  
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
		OutputDebugStringA("找不到进程\n");
		return false;
	}
	// 打开目标进程  
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessID);
	if (hProcess == NULL)
	{
		OutputDebugStringA("打开进程失败\n");
		return false;
	}

	// 使目标进程调用FreeLibrary，卸载DLL  
	LPVOID pFunc = FreeLibrary;
	MODULEENTRY32 me;
	if (!FindProcessModule(lpszProcess, lpszDllName, &me))
	{
		CloseHandle(hProcess);

		sprintf_s(szOutputBuf, "获取模块句柄失败: %s\n", lpszDllName);
		OutputDebugStringA(szOutputBuf);
		return false;
	}
	DWORD dwID, dwCode;
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, me.modBaseAddr, 0, &dwID);
	if (hThread == NULL)
	{
		CloseHandle(hProcess);

		sprintf_s(szOutputBuf, "创建远程线程失败:%d\n", GetLastError());
		OutputDebugStringA(szOutputBuf);
		return false;
	}
	// 等待FreeLibrary卸载完毕  
	WaitForSingleObject(hThread, INFINITE);
	// 获得FreeLibrary的返回值  
	GetExitCodeThread(hThread, &dwCode);
	if (dwCode == 0)
	{
		CloseHandle(hThread);
		DWORD dwError = GetRemoteLastError(hProcess);
		CloseHandle(hProcess);

		sprintf_s(szOutputBuf, "卸载DLL失败 Err Code: %d\n", dwError);
		OutputDebugStringA(szOutputBuf);
		return false;
	}
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return true;
}