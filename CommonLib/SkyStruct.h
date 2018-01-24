#pragma once
#include "windef.h"

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
    

