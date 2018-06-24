#pragma once
#include "windef.h"

struct tm
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

#ifndef _SYSTIME_DEFINED
#define _SYSTIME_DEFINED
struct _SYSTEMTIME
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
};
typedef struct _SYSTEMTIME SYSTEMTIME;
typedef SYSTEMTIME * LPSYSTEMTIME;
#endif

extern bool GetLocalTime(LPSYSTEMTIME lpSystemTime);