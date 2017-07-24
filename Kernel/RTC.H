/*
	Real Time Clock (Motorola MC146818)

	This program is writen by Sam on 4-Feb-2002
	You can get the latest version from web
			www.geocities.com/samuelhard
			samuelhard@yahoo.com

	Note :
		When intializing the kernel you must set 0x6 to the Status B
		register so that the RTC will not generate any interrupt and
		operate on Binary data mode and 24hrs settings.
*/
#ifndef __RTC__H
#define __RTC__H

#include "windef.h"
#include "Hal.h"

//RTC Command registers
#define RTC_INDEX_REG 0x70
#define RTC_VALUE_REG 0x71

//RTC Data register index
#define RTC_SECOND 0
#define RTC_MINUTE 2
#define RTC_HOUR   4

#define RTC_DAY_OF_WEEK  6

#define RTC_DAY   7
#define RTC_MONTH 8
#define RTC_YEAR  9

#define RTC_STATUS_A 0x0A
#define RTC_STATUS_B 0x0B
#define RTC_STATUS_C 0x0C
#define RTC_STATUS_D 0x0D

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
    
void GetLocalTime(LPSYSTEMTIME lpSystemTime);
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime);
 
#endif
