#include "KernelProcedure.h"
#include "SkyConsole.h"
#include "string.h"
#include "memory.h"
#include "Hal.h"
#include "PIT.h"
#include "Process.h"
#include "ProcessManager.h"
#include "SkyAPI.h"
#include "ConsoleManager.h"
#include "Scheduler.h"
#include "PhysicalMemoryManager.h"

extern bool systemOn;

void NativeConsole()
{
	systemOn = true;

	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);	

	ConsoleManager manager;

	char	commandBuffer[MAXPATH];

	while (1)
	{
		SkyConsole::Print("Command> ");
		memset(commandBuffer, 0, MAXPATH);
		//SkyConsole::Print("commandBuffer Address : 0x%x\n", &commandBuffer);	

		SkyConsole::GetCommand(commandBuffer, MAXPATH - 2);
		SkyConsole::Print("\n");

		if (manager.RunCommand(commandBuffer) == true)
			break;
	}
}

DWORD WINAPI SystemConsoleProc(LPVOID parameter)
{	
	SkyConsole::Print("Console Mode Start!!\n");
		
	NativeConsole();

	SkyConsole::Print("Bye!!");

	for (;;);

	return 0;
}

#define TS_WATCHDOG_CLOCK_POS		(0xb8000+(80-1)*2)
#define TIMEOUT_PER_SECOND		50
static bool m_bShowTSWatchdogClock = true;

DWORD WINAPI WatchDogProc(LPVOID parameter)
{		
	Process* pProcess = (Process*)parameter;
	int pos = 0;
	char *addr = (char *)TS_WATCHDOG_CLOCK_POS, status[] = { '-', '\\', '|', '/', '-', '\\', '|', '/' };
	int first = GetTickCount();

	while (1) 
	{
	
		int second = GetTickCount();
		if (second - first >= TIMEOUT_PER_SECOND)
		{
			if (++pos > 7) 
				pos = 0;
		
			if (m_bShowTSWatchdogClock)
				*addr = status[pos];

			first = GetTickCount();
		}
	}

	return 0;
}

DWORD WINAPI ProcessRemoverProc(LPVOID parameter)
{
	int static id = 0;
	int temp = id++;
	int first = GetTickCount();
	while (1)
	{
		kEnterCriticalSection();

		//ProcessManager::GetInstance()->RemoveProcess();
		//Scheduler::GetInstance()->Yield(kGetCurrentThreadId());
		VirtualMemoryManager::SetPageDirectory(VirtualMemoryManager::GetKernelPageDirectory());
		
		PhysicalMemoryManager::EnablePaging(false);
		//for (;;);
		PhysicalMemoryManager::EnablePaging(true);
		
		//VirtualMemoryManager::SetPageDirectory(VirtualMemoryManager::GetCurPageDirectory());
		
			int second = GetTickCount();
			if (second - first >= 400)
			{
				SkyConsole::Print("aaa %d\n", temp);

				first = GetTickCount();
			}
		

		kLeaveCriticalSection();
	}

	return 0;
}