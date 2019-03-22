#pragma once
#pragma pack(push, 1)
#include <stdint.h>

enum NSCMD
{
	eDATA = 1,		//数据
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

	struct DATA: public HEAD
	{
		CONSTRUCT(DATA, NSCMD::eDATA);
		union {
			int iparam1;
			int iparam2;
			char sparam1[32];
			char sparam2[32];
		} un;

		char data[1024];
	};
}


#pragma pack(pop)
