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
#include "SkyWindow.h"
#include "VideoRam.h"
#include "SkySimpleGUI.h"
#include "SkyGUISystem.h"

extern bool systemOn;

void StartGUISystem();

void NativeConsole()
{	
	ConsoleManager manager;

	char	commandBuffer[MAXPATH];

	//ProcessManager::GetInstance()->CreateProcessFromFile("Hello.exe", nullptr, PROCESS_USER);
	//ProcessManager::GetInstance()->CreateProcessFromFile("Hello.exe", nullptr, PROCESS_USER);
	//ProcessManager::GetInstance()->CreateProcessFromFile("Hello.exe", nullptr, PROCESS_USER);

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

	systemOn = true;
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	
	if (SkyGUISystem::GetInstance()->GUIEnable() == true)
	{
		StartGUISystem();
	}
	else
	{
		NativeConsole();
	}
			
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

	//SkyConsole::Print("\nWatchDogProc Start. Thread Id : %d\n", kGetCurrentThreadId());

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
		
//페이징 기능 전환 테스트		
		kEnterCriticalSection();
		//PhysicalMemoryManager::EnablePaging(false);
		//PhysicalMemoryManager::EnablePaging(true);

		ProcessManager::GetInstance()->RemoveTerminatedProcess();
		kLeaveCriticalSection();
		
			int second = GetTickCount();
			if (second - first >= 400)
			{
				first = GetTickCount();
			}
	}

	return 0;
}

void StartGUISystem()
{
	SkyWindow* pWindow = new SkySimpleGUI();
	VideoRamInfo& info = VideoRam::GetInstance()->GetVideoRamInfo();
	pWindow->Initialize(info._pVideoRamPtr, info._width, info._height, info._bpp);
	pWindow->Run();
}