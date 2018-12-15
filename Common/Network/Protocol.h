#pragma once
#pragma pack(push, 1)
#include "Cmd.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace NSPROTO
{
#define CONSTRUCT(name, _cmd) name() { size = sizeof(name); cmd = _cmd; }

	struct PROTO 
	{
		uint32_t size;
	};
	struct HEAD : public PROTO
	{
		uint16_t cmd;
	};

	struct LOGIN_REQ : public HEAD
	{
		CONSTRUCT(LOGIN_REQ, NSNETCMD::eLOGIN_REQ);
		int userid;
		char mac_addr[24];
	};

	struct LOGIN_RET : public HEAD
	{
		CONSTRUCT(LOGIN_RET, NSNETCMD::eLOGIN_RET);
		int userid;
		int8_t code;	//0成功; 1失败
	};

	struct BUILD_REQ : public HEAD
	{
		CONSTRUCT(BUILD_REQ, NSNETCMD::eBUILD_REQ);
		int client_userid;
		int server_userid;
	};

	struct BUILD_RET : public HEAD
	{
		CONSTRUCT(BUILD_RET, NSNETCMD::eBUILD_RET);
		int roomid;
		int8_t service; //1服务器; 2客户端
	};

	struct UNBUILD_REQ : public HEAD
	{
		CONSTRUCT(UNBUILD_REQ, NSNETCMD::eUNBUILD_REQ);
		int roomid;
	};

	struct UNBUILD_RET : public HEAD
	{
		CONSTRUCT(UNBUILD_RET, NSNETCMD::eUNBUILD_RET);
		int code;		//0成功; 1失败
		int roomid;
		int8_t service; //1服务器; 2客户端
	};

	struct FRAME_SYNC : public HEAD
	{
		typedef struct
		{
			int left;
			int top;
			int width;
			int height;
			int bitsPerPixel;
			int bufferSize;
			uint8_t* bufferPtr;
		}FRAME;

		FRAME_SYNC()
		{
			size = sizeof(FRAME_SYNC)-sizeof(dataPtr);
			cmd = NSNETCMD::eFRAME_SYNC;
			srv_uid = 0;
			frameNum = 0;
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
		int srv_uid;
		uint16_t frameNum;
		uint8_t* dataPtr;

		void setBuffer(int left, int top, int width, int height, int bitsPerPixel, const uint8_t* _buffer, int _bufferSize)
		{
			int oldSize = size;

			FRAME frame;
			frame.left = left;
			frame.top = top;
			frame.width = width;
			frame.height = height;
			frame.bitsPerPixel = bitsPerPixel;
			frame.bufferSize = _bufferSize;
			int frameSize = sizeof(frame)-sizeof(frame.bufferPtr);

			size += frameSize + frame.bufferSize;
			dataPtr = (uint8_t*)realloc(dataPtr, size);
			if (frameNum == 0)
			{
				memcpy(dataPtr, (uint8_t*)this, oldSize);
			}
			memcpy(dataPtr + oldSize, &frame, frameSize);
			memcpy(dataPtr + oldSize + frameSize, _buffer, _bufferSize);

			FRAME_SYNC& sync = *(FRAME_SYNC*)dataPtr;
			sync.size = size;
			sync.frameNum = ++frameNum;
		}
		void getData(uint8_t** _dataPtr, int& _dataSize)
		{
			*_dataPtr = dataPtr;
			_dataSize = size;
		}
	};
}


#pragma pack(pop)
