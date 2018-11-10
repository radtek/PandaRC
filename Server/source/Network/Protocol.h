#pragma once
#pragma pack(push, 1)
#include "Cmd.h"
#include <stdint.h>

namespace PROTO
{
#define CONSTRUCT(name) name() { size = sizeof(name); }

	struct HEAD
	{
		uint16_t cmd;
		uint16_t size;
	};

	struct LOGIN : public HEAD
	{
		CONSTRUCT(LOGIN);
		char mac_addr[24];
	};

	struct BUILD : public HEAD
	{
		CONSTRUCT(BUILD);
		char mac_client[24];
		char mac_server[24];
	};
}


#pragma pack(pop)
