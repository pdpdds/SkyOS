#pragma once
#include "windef.h"
#include "SkyStruct.h"
#include "Hal.h"
#include "PIT.h"
#include "time.h"

#define ASSERT(a, b) if(a == false) SkyConsole::Print("Kernel Panic : %s\n", b); _asm hlt

#define SKY_ASSERT(Expr, Msg) \
    __SKY_ASSERT(#Expr, Expr, __FILE__, __LINE__, Msg)

void __SKY_ASSERT(const char* expr_str, bool expr, const char* file, int line, const char* msg);


#define kEnterCriticalSection()	__asm	PUSHFD	__asm CLI
#define kLeaveCriticalSection()		__asm	POPFD

/////////////////////////////////////////////////////////////////////////////
//½º·¹µå
/////////////////////////////////////////////////////////////////////////////
DWORD SKYAPI kGetCurrentThreadId();

bool GetLocalTime(LPSYSTEMTIME lpSystemTime);
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime);

void printf(const char* str, ...);
int kprintf(const char* str);
void ksleep(int millisecond);
HANDLE CreateThread(SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreateionFlags, LPDWORD lpThreadId);
void PauseSystem(const char* msg);