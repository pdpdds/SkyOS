#pragma once
#include "windef.h"
#include "SkyStruct.h"
#include "Hal.h"
#include "PIT.h"
#include "time.h"

#define ASSERT(a, b) if(a == false) SkyConsole::Print("Kernel Panic : %s\n", b); _asm hlt

#define M_Assert(Expr, Msg) \
    __M_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)

void __M_Assert(const char* expr_str, bool expr, const char* file, int line, const char* msg);

#define kprintf SkyConsole::Print
#define kEnterCriticalSection()	__asm	PUSHFD	__asm CLI
#define kLeaveCriticalSection()		__asm	POPFD

/////////////////////////////////////////////////////////////////////////////
//동기화
/////////////////////////////////////////////////////////////////////////////
typedef struct _CRITICAL_SECTION {
	
	LONG LockRecursionCount;
	HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
	      
} CRITICAL_SECTION, *LPCRITICAL_SECTION;;


/////////////////////////////////////////////////////////////////////////////
//스레드
/////////////////////////////////////////////////////////////////////////////
DWORD SKYAPI kGetCurrentThreadId();

bool GetLocalTime(LPSYSTEMTIME lpSystemTime);
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime);

void printf(const char* str, ...);
void ksleep(int millisecond);
HANDLE CreateThread(SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreateionFlags, LPDWORD lpThreadId);
void PauseSystem(const char* msg);