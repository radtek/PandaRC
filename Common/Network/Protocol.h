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

	struct FRAME_SYNC : public HEAD
	{
		FRAME_SYNC()
		{
			size = sizeof(FRAME_SYNC)-sizeof(dataPtr);
			cmd = NSNETCMD::eFRAME_SYNC;
			left = 0;
			top = 0;
			width = 0;
			height = 0;
			dataSize = 0;
			dataPtr = NULL;
		}
		~FRAME_SYNC()
		{
			if (dataPtr != NULL)
			{
				free(dataPtr);
				dataPtr = NULL;
			}
		}
		int left;
		int top;
		int width;
		int height;

		int dataSize;
		uint8_t* dataPtr;

		void setData(uint8_t* _dataPtr, int _dataSize)
		{
			dataSize = _dataSize;
			dataPtr = (uint8_t*)realloc(dataPtr, size + dataSize);
			memcpy(dataPtr, (uint8_t*)this, size);
			memcpy(dataPtr + size, _dataPtr, dataSize);
			size += dataSize;
		}
		void getData(uint8_t** _dataPtr, int& _dataSize)
		{
			*_dataPtr = dataPtr;
			_dataSize = dataSize;
		}
	};
}


#pragma pack(pop)
