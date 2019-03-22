#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#include <stdio.h>

//自定义APIHOOK结构
typedef struct
{
	FARPROC funcaddr;
	BYTE olddata[5];
	BYTE newdata[5];
}HOOKSTRUCT;

HHOOK g_hHook;
HINSTANCE g_hinstDll;
HMODULE hModule;
HWND g_hForm; //接收信息窗口句柄
DWORD dwIdOld, dwIdNew;

//------------------------------------------------------------------------
// 由于要截获两个库里面的函数,所以每个函数定义了两个HOOK结构
// 在编程过程中因为没有考虑到这个问题,导致很多包没有截获到,
// 后来想到了冰哥在模仿SOCKCAP的程序中每个函数截了两次才明白
// 一个是wsock32.dll, 一个是ws2_32.dll
//------------------------------------------------------------------------
HOOKSTRUCT recvapi;
HOOKSTRUCT recvapi1;
HOOKSTRUCT sendapi;
HOOKSTRUCT sendapi1;
HOOKSTRUCT sendtoapi;
HOOKSTRUCT sendtoapi1;
HOOKSTRUCT WSASendapi;

bool hookapi(char *dllname, char *procname, DWORD_PTR myfuncaddr, HOOKSTRUCT *hookfunc);
int WINAPI Myrecv(SOCKET s, char FAR *buf, int len, int flags);
int WINAPI Myrecv1(SOCKET s, char FAR *buf, int len, int flags);
int WINAPI Mysend(SOCKET s, char FAR *buf, int len, int flags);
int WINAPI Mysend1(SOCKET s, char FAR *buf, int len, int flags);
int WINAPI Mysendto(SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen);
int WINAPI Mysendto1(SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen);
int WINAPI MyWSASend(
	SOCKET s,
	LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent,
	DWORD dwFlags,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);
bool sndmsg(char *buf);

void HookOn();
void HookOff();
bool InitHook();

//---------------------------------------------------------------------------
LRESULT WINAPI Hook(int nCode, WPARAM wParam, LPARAM lParam)
{
	return(CallNextHookEx(g_hHook, nCode, wParam, lParam));
}
//---------------------------------------------------------------------------
__declspec(dllexport) bool _stdcall InstallHook()
{
	DWORD dwProcessID = 0;
	GetWindowThreadProcessId(g_hForm, &dwProcessID);
	g_hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)Hook, g_hinstDll, dwProcessID);
	if (!g_hHook)
	{
		MessageBoxA(NULL, "SET ERROR", "ERROR", MB_OK);
		return(false);
	}
	return(true);
}
//---------------------------------------------------------------------------
__declspec(dllexport) bool _stdcall UninstallHook()
{
	HookOff();
	if (g_hHook == NULL)
		return true;
	return (UnhookWindowsHookEx(g_hHook) != 0);
}

//---------------------------------------------------------------------------
// 根据输入结构截获API
//---------------------------------------------------------------------------
bool hookapi(char *dllname, char *procname, DWORD_PTR myfuncaddr, HOOKSTRUCT *hookfunc)
{
	hModule = LoadLibrary(dllname);
	hookfunc->funcaddr = GetProcAddress(hModule, procname);
	if (hookfunc->funcaddr == NULL)
		return false;

	memcpy(hookfunc->olddata, hookfunc->funcaddr, 5);
	hookfunc->newdata[0] = 0xe9; //JMP指令
	DWORD_PTR jmpaddr = myfuncaddr - (DWORD_PTR)hookfunc->funcaddr - 5;
	memcpy(&hookfunc->newdata[1], &jmpaddr, 5);
	return true;
}
//---------------------------------------------------------------------------
void HookOnOne(HOOKSTRUCT *hookfunc)
{
	HANDLE hProc;
	dwIdOld = dwIdNew;
	hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwIdOld);
	VirtualProtectEx(hProc, hookfunc->funcaddr, 5, PAGE_READWRITE, &dwIdOld);
	WriteProcessMemory(hProc, hookfunc->funcaddr, hookfunc->newdata, 5, 0);
	VirtualProtectEx(hProc, hookfunc->funcaddr, 5, dwIdOld, &dwIdOld);
}
//---------------------------------------------------------------------------
void HookOn()
{
	HookOnOne(&recvapi);
	HookOnOne(&sendapi);
	HookOnOne(&sendtoapi);
	HookOnOne(&recvapi1);
	HookOnOne(&sendapi1);
	HookOnOne(&sendtoapi1);
	HookOnOne(&WSASendapi);
}
//---------------------------------------------------------------------------
void HookOffOne(HOOKSTRUCT *hookfunc)
{
	HANDLE hProc;
	dwIdOld = dwIdNew;
	hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwIdOld);
	VirtualProtectEx(hProc, hookfunc->funcaddr, 5, PAGE_READWRITE, &dwIdOld);
	WriteProcessMemory(hProc, hookfunc->funcaddr, hookfunc->olddata, 5, 0);
	VirtualProtectEx(hProc, hookfunc->funcaddr, 5, dwIdOld, &dwIdOld);
}

//---------------------------------------------------------------------------
void HookOff()
{
	HookOffOne(&recvapi);
	HookOffOne(&sendapi);
	HookOffOne(&sendtoapi);
	HookOffOne(&recvapi1);
	HookOffOne(&sendapi1);
	HookOffOne(&sendtoapi1);
	HookOffOne(&WSASendapi);
}
//---------------------------------------------------------------------------
int WINAPI Myrecv(SOCKET s, char FAR *buf, int len, int flags)
{
	int nReturn;
	HookOffOne(&recvapi);
	nReturn = recv(s, buf, len, flags);
	HookOnOne(&recvapi);

	char *tmpbuf = new char[len + 100];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	sprintf(tmpbuf, "recv|%d|%d|%s", GetCurrentProcessId(), len, buf);
	sndmsg(tmpbuf);
	delete tmpbuf;
	return(nReturn);
}
//---------------------------------------------------------------------------
int WINAPI Myrecv1(SOCKET s, char FAR *buf, int len, int flags)
{
	int nReturn;
	HookOffOne(&recvapi1);
	nReturn = recv(s, buf, len, flags);
	HookOnOne(&recvapi1);

	char * const tmpbuf = new char[len + 100];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	sprintf(tmpbuf, "recv1|%d|%d|%s", GetCurrentProcessId(), len, buf);
	sndmsg(tmpbuf);
	delete tmpbuf;
	return(nReturn);
}
//---------------------------------------------------------------------------
int WINAPI Mysend(SOCKET s, char FAR *buf, int len, int flags)
{
	int nReturn;
	HookOffOne(&sendapi);
	nReturn = send(s, buf, len, flags);
	HookOnOne(&sendapi);

	char *tmpbuf = new char[len + 100];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	sprintf(tmpbuf, "send|%d|%d|%s",
		GetCurrentProcessId(),
		len,
		buf);
	sndmsg(tmpbuf);
	delete tmpbuf;
	return(nReturn);
}
//---------------------------------------------------------------------------
int WINAPI Mysend1(SOCKET s, char FAR *buf, int len, int flags)
{
	int nReturn;
	HookOffOne(&sendapi1);
	nReturn = send(s, buf, len, flags);
	HookOnOne(&sendapi1);

	char *tmpbuf = new char[len + 100];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	sprintf(tmpbuf, "send1|%d|%d|%s",
		GetCurrentProcessId(),
		len,
		buf);
	sndmsg(tmpbuf);
	delete tmpbuf;
	return(nReturn);
}
//--------------------------------------------------------------------------
int WINAPI Mysendto(SOCKET s, const char FAR * buf, int len,
	int flags, const struct sockaddr FAR * to, int tolen)
{
	int nReturn;
	HookOffOne(&sendtoapi);
	nReturn = sendto(s, buf, len, flags, to, tolen);
	HookOnOne(&sendtoapi);

	char *tmpbuf = new char[len + 100];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	sprintf(tmpbuf, "sendto|%d|%d|%s",
		GetCurrentProcessId(),
		len,
		buf);
	sndmsg(tmpbuf);
	delete tmpbuf;
	return(nReturn);
}
//--------------------------------------------------------------------------
int WINAPI Mysendto1(SOCKET s, const char FAR * buf, int len,
	int flags, const struct sockaddr FAR * to, int tolen)
{
	int nReturn;
	HookOffOne(&sendtoapi1);
	nReturn = sendto(s, buf, len, flags, to, tolen);
	HookOnOne(&sendtoapi1);

	char *tmpbuf = new char[len + 100];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	sprintf(tmpbuf, "sendto1|%d|%d|%s",
		GetCurrentProcessId(),
		len,
		buf);
	sndmsg(tmpbuf);
	delete tmpbuf;
	return(nReturn);
}
//----------------------------------------------------------------------------
int WINAPI MyWSASend(
	SOCKET s,
	LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent,
	DWORD dwFlags,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	int nReturn;
	HookOffOne(&WSASendapi);
	nReturn = WSASend(s, lpBuffers, dwBufferCount,
		lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
	HookOnOne(&WSASendapi);

	char *tmpbuf = new char[*lpNumberOfBytesSent + 100];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	sprintf(tmpbuf, "WSASend|%d|%d|%s", GetCurrentProcessId(), *lpNumberOfBytesSent, lpBuffers->buf);
	sndmsg(tmpbuf);
	delete tmpbuf;
	return(nReturn);
}

//-----------------------------------------------------------------
// 向窗口发送消息
// 考虑到简单性,用了COPYDATASTRUCT结构
// 用内存映射应该会快一点
//-----------------------------------------------------------------
bool sndmsg(char *buf)
{
	COPYDATASTRUCT cds;
	cds.dwData = sizeof(COPYDATASTRUCT);
	cds.cbData = (DWORD)strlen(buf);
	cds.lpData = buf;
	SendMessage(g_hForm, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
	return true;
}

//-----------------------------------------------------------------------
bool InitHook()
{
	hookapi("wsock32.dll", "recv", (DWORD_PTR)Myrecv, &recvapi);
	hookapi("ws2_32.dll", "recv", (DWORD_PTR)Myrecv1, &recvapi1);
	hookapi("wsock32.dll", "send", (DWORD_PTR)Mysend, &sendapi);
	hookapi("ws2_32.dll", "send", (DWORD_PTR)Mysend1, &sendapi1);
	hookapi("wsock32.dll", "sendto", (DWORD_PTR)Mysendto, &sendtoapi);
	hookapi("ws2_32.dll", "sendto", (DWORD_PTR)Mysendto1, &sendtoapi1);
	hookapi("wsock32.dll", "WSASend", (DWORD_PTR)MyWSASend, &WSASendapi);
	dwIdNew = GetCurrentProcessId(); // 得到所属进程的ID
	dwIdOld = dwIdNew;
	HookOn(); // 开始拦截
	return(true);
}





//主窗体代码:
//---------------------------------------------------------------------------

//#include <vcl.h>
//#pragma hdrstop
//
//#include "main_Form.h"
////---------------------------------------------------------------------------
//#pragma package(smart_init)
//#pragma link "HexEdit"
//#pragma resource "*.dfm"
//TForm1 *Form1;
//
//HINSTANCE hdll;
//bool __stdcall (*InstallHook)();
//bool __stdcall (*UninstallHook)();
////---------------------------------------------------------------------------
//__fastcall TForm1::TForm1(TComponent* Owner)
//	: TForm(Owner)
//{
//	Application->OnHint = DisplayHint;
//}
////---------------------------------------------------------------------------
//void __fastcall TForm1::Button1Click(TObject *Sender)
//{
//	g_dindex = 0;
//
//	hdll = LoadLibrary("dll.dll");
//	if (hdll == NULL)
//		MessageBox(NULL, "LoadLibrary", "Error", MB_OK | MB_ICONERROR);
//	InstallHook = GetProcAddress(hdll, "InstallHook");
//	if (!InstallHook)
//	{
//		MessageBox(NULL, "InstallHook", "Error", MB_OK | MB_ICONERROR);
//	}
//	UninstallHook = GetProcAddress(hdll, "UninstallHook");
//	if (!UninstallHook)
//	{
//		MessageBox(NULL, "UninstallHook", "Error", MB_OK | MB_ICONERROR);
//	}
//	InstallHook();
//
//	startBtn->Enabled = false;
//	stopBtn->Enabled = true;
//}
////---------------------------------------------------------------------------
//void __fastcall TForm1::Button2Click(TObject *Sender)
//{
//	g_dindex = 0;
//	UninstallHook();
//	FreeLibrary(hdll);
//	startBtn->Enabled = true;
//	stopBtn->Enabled = false;
//}
////---------------------------------------------------------------------------
//void __fastcall TForm1::OnCopyData(TMessage &Msg)
//{
//	COPYDATASTRUCT *cds = (COPYDATASTRUCT*)Msg.LParam;
//	AnsiString tmpbuf = (char *)cds->lpData;
//	TListItem *li = lv->Items->Add();
//	li->Caption = g_dindex;
//	if (tmpbuf.SubString(1, tmpbuf.Pos("|") - 1).Pos("send")>0)
//	{
//		li->ImageIndex = 1;
//	}
//	else
//	{
//		li->ImageIndex = 0;
//	}
//
//	li->SubItems->Add(tmpbuf.SubString(1, tmpbuf.Pos("|") - 1));
//	tmpbuf = tmpbuf.SubString(tmpbuf.Pos("|") + 1, tmpbuf.Length());
//	li->SubItems->Add(tmpbuf.SubString(1, tmpbuf.Pos("|") - 1));
//	tmpbuf = tmpbuf.SubString(tmpbuf.Pos("|") + 1, tmpbuf.Length());
//	li->SubItems->Add(tmpbuf.SubString(1, tmpbuf.Pos("|") - 1));
//	li->SubItems->Add(tmpbuf.SubString(tmpbuf.Pos("|") + 1, tmpbuf.Length()));
//}
//
//void __fastcall TForm1::lvInsert(TObject *Sender, TListItem *Item)
//{
//	g_dindex++;
//	lv->Perform(LVM_SCROLL, 0, 10);
//}
////---------------------------------------------------------------------------
//
//void __fastcall TForm1::lvClick(TObject *Sender)
//{
//	if (lv->ItemIndex < 0)
//		return;
//	HexEdit1->LoadFromBuffer(lv->Items->Item[lv->ItemIndex]->SubItems->Strings[3].c_str(),
//		lv->Items->Item[lv->ItemIndex]->SubItems->Strings[3].Length());
//}
////---------------------------------------------------------------------------
//
//void __fastcall TForm1::SpeedButton3Click(TObject *Sender)
//{
//	lv->Clear();
//}
////---------------------------------------------------------------------------
//
//void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
//{
//	if (stopBtn->Enabled)
//		Button2Click(Sender);
//}
////---------------------------------------------------------------------------
//
//void __fastcall TForm1::lvAdvancedCustomDrawItem(TCustomListView *Sender,
//	TListItem *Item, TCustomDrawState State, TCustomDrawStage Stage,
//	bool &DefaultDraw)
//{
//	if (Item->ImageIndex == 0)
//	{
//		lv->Canvas->Brush->Color = 0x00FFF5EC;
//	}
//}
////---------------------------------------------------------------------------
//
//void __fastcall TForm1::lvKeyUp(TObject *Sender, WORD &Key,
//	TShiftState Shift)
//{
//	if (lv->ItemIndex < 0)
//		return;
//	HexEdit1->LoadFromBuffer(lv->Items->Item[lv->ItemIndex]->SubItems->Strings[3].c_str(),
//		lv->Items->Item[lv->ItemIndex]->SubItems->Strings[3].Length());
//}
////---------------------------------------------------------------------------
//void __fastcall TForm1::DisplayHint(TObject *Sender)
//{
//	StatusBar1->SimpleText = GetLongHint(Application->Hint);
//}
