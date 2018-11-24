#pragma once
#pragma pack(push, 1)
#include "Cmd.h"
#include <stdint.h>

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

	struct LOGIN_REQ : public HEAD
	{
		CONSTRUCT(LOGIN_REQ, NSNETCMD::eLOGIN_REQ);
		char mac_addr[24];
	};

	struct LOGIN_RET : public HEAD
	{
		CONSTRUCT(LOGIN_RET, NSNETCMD::eLOGIN_RET);
		int8_t code;	//0成功; 1失败
	};

	struct BUILD_REQ : public HEAD
	{
		CONSTRUCT(BUILD_REQ, NSNETCMD::eBUILD_REQ);
		char mac_client[24];
		char mac_server[24];
	};

	struct BUILD_RET : public HEAD
	{
		CONSTRUCT(BUILD_RET, NSNETCMD::eBUILD_RET);
		int roomid;
	};

	struct UNBUILD_REQ : public HEAD
	{
		CONSTRUCT(UNBUILD_REQ, NSNETCMD::eUNBUILD_REQ);
		int roomid;
	};

	struct UNBUILD_RET : public HEAD
	{
		CONSTRUCT(UNBUILD_RET, NSNETCMD::eUNBUILD_RET);
		int code;	//0成功; 1失败
	};
}


#pragma pack(pop)
