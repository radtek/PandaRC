#ifndef __PLATFORM_FN_H__
#define __PLATFORM_FN_H__

#include "Common/PlatformHeader.h"
#include "Common/DataStruct/Encoding.h"

namespace Platform
{
	inline bool FileExist(const char* psFile)
	{
		FILE* poFile = fopen(psFile, "r");
		if (poFile != NULL)
		{
			fclose(poFile);
			return true;
		}
		return false;
	}

#ifdef _WIN32
	inline const char* LastErrorStr(int nLastErrCode)
	{
		// Retrieve the system error message for the last-error code
		static char sMsgBuf[1024];
		memset(sMsgBuf, 0, sizeof(sMsgBuf));
		LPVOID lpMsgBuf = NULL;
		int nLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			nLastErrCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		if (nLen > 0)
		{
			Encoding::GBKToUTF8((char*)lpMsgBuf, sMsgBuf, sizeof(sMsgBuf));
		}
		if (lpMsgBuf != NULL)
		{
			LocalFree(lpMsgBuf);
		}
		return sMsgBuf;
	}
#endif
};

#endif
