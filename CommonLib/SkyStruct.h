#pragma once
#include "windef.h"
#include "stdint.h"
#include "string.h"
#include "memory.h"
#include "ctype.h"
#include "list.h"
//#include "hash_map.h"
#include "map.h"
#include "stack.h"
#include "vector.h"
#include "deque.h"
#include "queue.h"
#include "stl_string.h"

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


    

