#pragma once

#if defined WIN32 || defined _WIN32 || defined WINCE
#include<windows.h>
#elif defined __linux || defined __linux__
#include <timer.h>
#endif

__int64 getTickCount(void)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	LARGE_INTEGER counter;
	QueryPerformanceCounter( &counter );
	return (__int64)counter.QuadPart;
#elif defined __linux || defined __linux__
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return (int64)tp.tv_sec*1000000000 + tp.tv_nsec;
#elif defined __MACH__ && defined __APPLE__
	return (int64)mach_absolute_time();
#else
	struct timeval tv;
	struct timezone tz;
	gettimeofday( &tv, &tz );
	return (int64)tv.tv_sec*1000000 + tv.tv_usec;
#endif
}



double getTickFrequency(void)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return (double)freq.QuadPart;
#elif defined __linux || defined __linux__
	return 1e9;
#elif defined __MACH__ && defined __APPLE__
	static double freq = 0;
	if( freq == 0 )
	{
		mach_timebase_info_data_t sTimebaseInfo;
		mach_timebase_info(&sTimebaseInfo);
		freq = sTimebaseInfo.denom*1e9/sTimebaseInfo.numer;
	}
	return freq;
#else
	return 1e6;
#endif
}

#define DECLARE_TIMING(s) __int64 timeStart_##s; double timeDiff_##s; double timeTally_##s = 0; int countTally_##s = 0 

#define START_TIMING(s) timeStart_##s = getTickCount() 

#define STOP_TIMING(s) timeDiff_##s = (double)(getTickCount() - timeStart_##s); timeTally_##s += timeDiff_##s; countTally_##s++ 

#define GET_TIMING(s) (double)(timeDiff_##s / (getTickFrequency()*1000.0)) 

#define GET_AVERAGE_TIMING(s) (double)(countTally_##s ? timeTally_##s/ ((double)countTally_##s * getTickFrequency()*1000.0) : 0) 

#define CLEAR_AVERAGE_TIMING(s) timeTally_##s = 0; countTally_##s = 0