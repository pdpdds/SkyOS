#pragma once
#include "windef.h"
#include "SkyStruct.h"
#include "Hal.h"
#include "PIT.h"
#include "time.h"

void SKYASSERT(bool result, const char* pMsg);

/////////////////////////////////////////////////////////////////////////////
//동기화
/////////////////////////////////////////////////////////////////////////////
typedef struct _CRITICAL_SECTION {
	
	LONG LockRecursionCount;
	HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
	      
} CRITICAL_SECTION, *LPCRITICAL_SECTION;;

extern CRITICAL_SECTION g_criticalSection;

#define kEnterCriticalSection()	__asm	PUSHFD	__asm CLI
#define kLeaveCriticalSection()		__asm	POPFD

/////////////////////////////////////////////////////////////////////////////
//스레드
/////////////////////////////////////////////////////////////////////////////
DWORD SKYAPI kGetCurrentThreadId();

bool GetLocalTime(LPSYSTEMTIME lpSystemTime);
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime);

void printf(const char* str, ...);
int kprintf(const char* str);
void ksleep(int millisecond);
HANDLE CreateThread(SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreateionFlags, LPDWORD lpThreadId);
void PauseSystem(const char* msg);