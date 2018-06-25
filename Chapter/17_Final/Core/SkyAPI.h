#pragma once
#include "windef.h"
#include "SkyStruct.h"
#include "SysError.h"
#include "time.h"

#define ASSERT(a, b) if(a == false) SkyConsole::Print("Kernel Panic : %s\n", b); _asm hlt

#define SKY_ASSERT(Expr, Msg) \
    __SKY_ASSERT(#Expr, Expr, __FILE__, __LINE__, Msg)

void __SKY_ASSERT(const char* expr_str, bool expr, const char* file, int line, const char* msg);

#define kprintf SkyConsole::Print
#define kEnterCriticalSection()	__asm	PUSHFD	__asm CLI
#define kLeaveCriticalSection()		__asm	POPFD

/////////////////////////////////////////////////////////////////////////////
//µ¿±âÈ­
/////////////////////////////////////////////////////////////////////////////
typedef struct _CRITICAL_SECTION {
	
	LONG LockRecursionCount;
	HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
	      
} CRITICAL_SECTION, *LPCRITICAL_SECTION;;


DWORD SKYAPI kGetCurrentThreadId();

bool GetLocalTime(LPSYSTEMTIME lpSystemTime);
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime);

DWORD GetLastError();
DWORD SetLastError(DWORD dwErrorCode);


void printf(const char* str, ...);
void DumpMemory(void *data, size_t nbytes, size_t bytes_per_line);
void ksleep(int millisecond);
HANDLE CreateThread(SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreateionFlags, LPDWORD lpThreadId);
void PauseSystem(const char* msg);

extern "C"
{
	int kdbg_printf(char *pFmt, ...);
	void TerminateProcess();
	uint32_t MemoryAlloc(size_t size);
	void MemoryFree(void* p);
	void CreateDefaultHeap();

	void* GetSkyAPIEntryPoint(char * strAPIName);
	void RegisterSkyAPI(char* strAPIName, void * ptrAPIFunction);
}