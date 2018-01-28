#include "SkyAPI.h"
#include "SkyConsole.h"
#include "ProcessManager.h"
#include "string.h"
#include "va_list.h"
#include "stdarg.h"
#include "sprintf.h"

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
//스레드
/////////////////////////////////////////////////////////////////////////////
DWORD SKYAPI kGetCurrentThreadId()
{
	return 0;
}

/* This function will return the current system date & time in the passed pointer parameters.

Note : The year is only 2 digits and the RTC is Real Time Clock  and donot confuse it with the Y2K problem.

The Day of week problem is still unsolved
*/
void GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
	/* Checking whether we can read the time now or not according to some documentation the MSB in Status A will remain 1 (invalid time) only a millisecond*/
	int TimeOut;

	OutPortByte(RTC_INDEX_REG, RTC_STATUS_A);    //check status - read access
	TimeOut = 1000;
	while (InPortByte(RTC_VALUE_REG) & 0x80)
		if (TimeOut < 0)
			return;
		else
			TimeOut--;

	OutPortByte(RTC_INDEX_REG, RTC_DAY);         //get day value
	lpSystemTime->wDay = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_MONTH);       //get month value
	lpSystemTime->wMonth = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_YEAR);        //get year
	lpSystemTime->wYear = InPortByte(RTC_VALUE_REG);

	OutPortByte(RTC_INDEX_REG, RTC_DAY_OF_WEEK); //get day of week - **** problem
	lpSystemTime->wDayOfWeek = InPortByte(RTC_VALUE_REG);

	OutPortByte(RTC_INDEX_REG, RTC_SECOND);
	lpSystemTime->wSecond = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_MINUTE);
	lpSystemTime->wMinute = InPortByte(RTC_VALUE_REG);
	OutPortByte(RTC_INDEX_REG, RTC_HOUR);
	lpSystemTime->wHour = InPortByte(RTC_VALUE_REG);

	OutPortByte(RTC_INDEX_REG, RTC_STATUS_B);
	OutPortByte(RTC_VALUE_REG, 2);


	lpSystemTime->wYear = (lpSystemTime->wYear / 16) * 10 + (lpSystemTime->wYear % 16);
	lpSystemTime->wMonth = (lpSystemTime->wMonth / 16) * 10 + (lpSystemTime->wMonth % 16);
	lpSystemTime->wDay = (lpSystemTime->wDay / 16) * 10 + (lpSystemTime->wDay % 16);
	lpSystemTime->wHour = (lpSystemTime->wHour / 16) * 10 + (lpSystemTime->wHour % 16);
	lpSystemTime->wMinute = (lpSystemTime->wMinute / 16) * 10 + (lpSystemTime->wMinute % 16);
	lpSystemTime->wSecond = (lpSystemTime->wSecond / 16) * 10 + (lpSystemTime->wSecond % 16);

	return;
}
/* Returns 1 on success and 0 on failue */
BYTE SetLocalTime(LPSYSTEMTIME lpSystemTime)
{
	/* Checking whether we can read the time now or not according to some documentation the MSB in Status A will   remain 1 (invalid time) only a millisecond*/
	int TimeOut;

	OutPortByte(RTC_INDEX_REG, RTC_STATUS_A);    //checking status -read access
	TimeOut = 1000;
	while (InPortByte(RTC_VALUE_REG) & 0x80)
		if (TimeOut< 0)
			return 0;
		else
			TimeOut--;

	OutPortByte(RTC_INDEX_REG, RTC_DAY);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wDay);
	OutPortByte(RTC_INDEX_REG, RTC_MONTH);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wMonth);
	OutPortByte(RTC_INDEX_REG, RTC_YEAR);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wYear);

	OutPortByte(RTC_INDEX_REG, RTC_SECOND);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wSecond);
	OutPortByte(RTC_INDEX_REG, RTC_MINUTE);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wMinute);
	OutPortByte(RTC_INDEX_REG, RTC_HOUR);
	OutPortByte(RTC_VALUE_REG, (uchar)lpSystemTime->wHour);

	return 1;
}

int kprintf(const char* str)
{
	SkyConsole::Write(str);
	return 0;
}

void printf(const char* str, ...)
{
	if(!str)
		return;

	va_list		args;
	va_start(args, str);
	size_t i;
	for (i = 0; i < strlen(str); i++) {

		switch (str[i]) {

		case '%':

			switch (str[i + 1]) {

				/*** characters ***/
			case 'c': {
				char c = va_arg(args, char);
				SkyConsole::WriteChar(c);
				i++;		// go to next character
				break;
			}

					  /*** address of ***/
			case 's': {
				int c = (int&)va_arg(args, char);
				char str[256];
				strcpy(str, (const char*)c);
				SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

					  /*** integers ***/
			case 'd':
			case 'i': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 10, str);
				SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

					  /*** display in hex ***/
					  /*int*/
			case 'X': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}
					  /*unsigned int*/
			case 'x': {
				unsigned int c = va_arg(args, unsigned int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

			default:
				va_end(args);
				return;
			}

			break;

		default:
			SkyConsole::WriteChar(str[i]);
			break;
		}

	}

	va_end(args);
	return;
}

HANDLE CreateThread(SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreateionFlags, LPDWORD lpThreadId)
{
	Process* cur = ProcessManager::GetInstance()->GetCurrentProcess();

	if (cur->m_processId == PROC_INVALID_ID)
	{
		SkyConsole::Print("Invailid Process Id\n");
		return 0;
	}

	Thread* newThread = ProcessManager::GetInstance()->CreateThread(cur, lpStartAddress, lpParameter);

	if (newThread == NULL)
	{
		SkyConsole::Print("Thread Create Fail!!\n");
		return 0;
	}

	return (HANDLE)newThread;
}

extern "C"
{
	uint32_t MemoryAlloc(size_t size)
	{
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		void *addr = alloc(size, (u8int)0, (heap_t*)pProcess->m_lpHeap);

#ifdef _ORANGE_DEBUG
		SkyConsole::Print("process heap alloc, %d %x\n", size, pProcess->m_lpHeap);
		SkyConsole::Print("process heap alloc, %d %x\n", size, pProcess->m_lpHeap);
#endif			
		return (u32int)addr;
	}

	void MemoryFree(void* p)
	{
		//힙은 스레드가 모두 공유한다.
		//따라서 메모리를 해제할시 컨텍스트 스위칭이 일어나서 다른 스레드가 같은 자원(힙)에 접근할 수 있는 가능성이 생기므로
		//인터럽트가 일어나지 않게 처리한다.
		EnterCriticalSection();
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		free(p, (heap_t*)pProcess->m_lpHeap);
		LeaveCriticalSection();
	}

	//프로세스 전용의 디폴트 힙을 생성한다
	void CreateDefaultHeap()
	{
		EnterCriticalSection();

		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		Thread* pThread = pProcess->GetThread(0);

		//1메가 바이트의 힙을 생성
		void* pHeapPhys = PhysicalMemoryManager::AllocBlocks(DEFAULT_HEAP_PAGE_COUNT);
		u32int heapAddess = pThread->m_imageBase + pThread->m_imageSize + PAGE_SIZE + PAGE_SIZE * 2;

		//힙 주소를 4K에 맞춰 Align	
		heapAddess -= (heapAddess % PAGE_SIZE);

#ifdef _ORANGE_DEBUG
		SkyConsole::Print("heap adress %x\n", heapAddess);
#endif // _ORANGE_DEBUG

		for (int i = 0; i < DEFAULT_HEAP_PAGE_COUNT; i++)
		{
			VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory,
				(uint32_t)heapAddess + i * PAGE_SIZE,
				(uint32_t)pHeapPhys + i * PAGE_SIZE,
				I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
		}

		memset((void*)heapAddess, 0, DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE);

		pProcess->m_lpHeap = create_heap((u32int)heapAddess, (uint32_t)heapAddess + DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE,
			(uint32_t)heapAddess + DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE, 0, 0);

		LeaveCriticalSection();
	}

	//프로세스 종료	
	extern "C" void TerminateProcess()
	{
		EnterCriticalSection();

		Process* cur = ProcessManager::GetInstance()->GetCurrentProcess();

		if (cur == NULL || cur->m_processId == PROC_INVALID_ID)
		{
			SkyConsole::Print("Invailid Process Termination\n");
			LeaveCriticalSection();
			return;
		}

		//프로세스 매니저에서 해당 프로세스를 완전히 제거한다.
		//태스크 목록에서도 제거되어 해당 프로세스는 더이상 스케쥴링 되지 않는다.		
		ProcessManager::GetInstance()->DestroyProcess(cur);

		LeaveCriticalSection();

		for (;;);
	}
}

void PauseSystem(const char* msg)
{
	for (;;);
}
