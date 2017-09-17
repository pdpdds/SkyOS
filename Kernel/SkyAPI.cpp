#include "SkyAPI.h"
#include "Console.h"

void SKYASSERT(bool result, const char* pMsg)
{
	if (result == false)
	{
		SkyConsole::Print("%s", pMsg);
		_asm hlt
	}
}

void SKYAPI kInitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	lpCriticalSection->LockRecursionCount = 0;
	lpCriticalSection->OwningThread = 0;
}

void SKYAPI kEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	_asm cli

	DWORD threadId = kGetCurrentThreadId();

	SKYASSERT((HANDLE)threadId == lpCriticalSection->OwningThread || lpCriticalSection->OwningThread == 0, "kEnterCriticalSection");

	if (lpCriticalSection->OwningThread == (HANDLE)threadId)
	{
		lpCriticalSection->LockRecursionCount++;
	}
	else
	{
		lpCriticalSection->OwningThread = (HANDLE)threadId;
		lpCriticalSection->LockRecursionCount = 1;
	}
}

void SKYAPI kLeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	DWORD threadId = kGetCurrentThreadId();

	SKYASSERT((HANDLE)threadId == lpCriticalSection->OwningThread, "kLeaveCriticalSection");

	lpCriticalSection->LockRecursionCount--;

	if (lpCriticalSection->LockRecursionCount == 0)
	{
		lpCriticalSection->OwningThread = 0;
		_asm sti
	}
}

/////////////////////////////////////////////////////////////////////////////
//½º·¹µå
/////////////////////////////////////////////////////////////////////////////
DWORD SKYAPI kGetCurrentThreadId()
{
	return 0;
}