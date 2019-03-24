#pragma once
#pragma pack(push, 1)
#include <stdint.h>

enum NSCMD
{
	eCLT_REG = 1,		//客户端注册
	eCLT_UNREG = 2,		//客户端反注册
	eCOM_ERR = 3,		//错误
	eCOM_DATA = 4,			//数据
	eSRV_EXITTHREAD = 5,	//通知客户端清理线程
};

namespace NSPROTO
{
#define CONSTRUCT(name, _cmd) name() { size = sizeof(name); cmd = _cmd; }

	struct PROTO
	{
		uint16_t size;
	};
	struct HEAD : public PROTO
	{
		uint16_t cmd;
	};
	struct CLT_REG: public HEAD
	{
		CONSTRUCT(CLT_REG, NSCMD::eCLT_REG);
		unsigned processid;
	};
	struct CLT_UNREG: public HEAD
	{
		CONSTRUCT(CLT_UNREG, NSCMD::eCLT_UNREG);
		unsigned processid;
	};
	struct COM_ERR: public HEAD
	{
		CONSTRUCT(COM_ERR, NSCMD::eCOM_ERR);
		unsigned processid;
		char errormsg[64];
	};
	struct COM_DATA: public HEAD
	{
		CONSTRUCT(COM_DATA, NSCMD::eCOM_DATA);
		char sparam1[32];
		uint32_t hostip;
		uint16_t hostport;
		uint32_t peerip;
		uint16_t peerport;
		int len;
		char data[0x7FFF];
	};
	struct SRV_EXITTHREAD: public HEAD
	{
		CONSTRUCT(SRV_EXITTHREAD, NSCMD::eSRV_EXITTHREAD);
	};
}


#pragma pack(pop)
