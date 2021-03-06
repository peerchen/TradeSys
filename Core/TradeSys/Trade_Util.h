
#ifndef TRADE_UTIL_H
#define TRADE_UTIL_H
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>

#if 0
#ifdef WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

#if 0
#ifdef WIN32
static inline int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;

	GetLocalTime(&wtm);
	tm.tm_year     = wtm.wYear - 1900;
	tm.tm_mon     = wtm.wMonth - 1;
	tm.tm_mday     = wtm.wDay;
	tm.tm_hour     = wtm.wHour;
	tm.tm_min     = wtm.wMinute;
	tm.tm_sec     = wtm.wSecond;
	tm. tm_isdst    = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;

	return (0);
}
#endif /* WIN32 */
#endif
#endif



static inline unsigned long long time_convert(int year, int month, int day, int hour, int min, int sec, int micro_sec)
{
	struct tm timein;
	timein.tm_year = year > 1980 ? (year - 1900) : year;
	timein.tm_mon = month-1;
	timein.tm_mday = day;//?
	timein.tm_hour = hour;
	timein.tm_min = min;
	timein.tm_sec = sec;
	time_t tm = mktime(&timein);

	//time_t tm = time(NULL);

	return (unsigned long long)tm*1000+micro_sec;
}

#endif
