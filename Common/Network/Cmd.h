#pragma once
enum NSNETCMD
{
	eLOGIN_REQ = 1,		//登录请求
	eLOGIN_RET = 2,		//登录返回
	eBUILD_REQ = 3,		//建立对话请求
	eBUILD_RET = 4,		//建立对话返回
	eUNBUILD_REQ = 5,	//拆除对话请求
	eUNBUILD_RET = 6,	//拆除对话返回
	eFRAME_SYNC = 7,	//同步视频帧
};