﻿#ifndef __TIME_API_H__
#define __TIME_API_H__

#include "Common/Platform.h"

//整数运算比浮点数运算快
//浮点数乘比浮点数除法快
namespace XTime
{
	// Microseconds since the system was started
	inline int64_t USTime(void)
	{
#ifdef __linux
		struct timespec tv; 
		clock_gettime(CLOCK_MONOTONIC, &tv);
		return (tv.tv_sec * (int64_t)1000000 + tv.tv_nsec / 1000);
#else
		LARGE_INTEGER TicksPerSecond;
		QueryPerformanceFrequency(&TicksPerSecond);
		LARGE_INTEGER Tick;
		QueryPerformanceCounter(&Tick);
		double fTicksDiv = TicksPerSecond.QuadPart * 0.000001;
		return (int64_t)(Tick.QuadPart / fTicksDiv);
#endif
	}

	// Milliseconds since the system was started
	inline int64_t MSTime(void)
	{
		return USTime() / 1000;
	}

	// Seconds since the system was started
	inline int Time()
	{
		return (int)(USTime() / 1000000);
	}

	// Seconds unix timestamp
	inline int SysTime()
	{
		return (int)time(0);
	}

	// Return the day id in one year
	inline int DayID()
	{
		time_t nTimeStamp = time(0);
		struct tm *pTm;
		pTm = localtime(&nTimeStamp);
		return pTm->tm_yday;
	}

#ifdef __linux
	// Return timespec
	inline struct timespec MakeTimespec(int nMS)
	{
		struct timespec Ts;
		clock_gettime(CLOCK_REALTIME, &Ts);
		Ts.tv_sec += nMS / 1000;
		Ts.tv_nsec += (nMS % 1000) * 1000000;
		Ts.tv_sec += Ts.tv_nsec / 1000000000;
		Ts.tv_nsec = Ts.tv_nsec % 1000000000;
		return Ts;
	}
#endif

	// Ms sleep
	inline void MSSleep(int nMillisec)
	{
#ifdef __linux
		usleep(nMillisec * 1000);
#else
		Sleep(nMillisec);
#endif
	}

	//标准时间,毫秒
	inline int64_t UnixMSTime()
	{
#ifdef _WIN32
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;
		::GetLocalTime(&wtm);
		tm.tm_year = wtm.wYear - 1900;
		tm.tm_mon = wtm.wMonth - 1;
		tm.tm_mday = wtm.wDay;
		tm.tm_hour = wtm.wHour;
		tm.tm_min = wtm.wMinute;
		tm.tm_sec = wtm.wSecond;
		tm.tm_isdst = -1;
		clock = mktime(&tm);

		int64_t nMSTime = clock * 1000 + wtm.wMilliseconds;
		return nMSTime;
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		int64_t nMSTime = (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
		return nMSTime;
#endif
	}
};




#endif
