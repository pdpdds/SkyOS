#include "SkyOS.h"
#include "SkyRenderer.h"

bool systemOn = false;

void NativeConsole()
{
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

	systemOn = true;
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	NativeConsole();

	SkyConsole::Print("Bye!!");

	return 0;
}

DWORD WINAPI SystemGUIProc(LPVOID parameter)
{
	systemOn = true;
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	SkyGUISystem::GetInstance()->Run();
	
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
		kEnterCriticalSection();
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());
		kLeaveCriticalSection();
	}

	return 0;
}

DWORD WINAPI GUIWatchDogProc(LPVOID parameter)
{
	Process* pProcess = (Process*)parameter;
	int pos = 0;
	int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF};
	int first = GetTickCount();

	while (1)
	{

		int second = GetTickCount();
		if (second - first >= TIMEOUT_PER_SECOND)
		{
			if (++pos > 2)
				pos = 0;

			if (m_bShowTSWatchdogClock)
				SkyGUIConsole::FillRect(1004, 0, 20, 20, colorStatus[pos], 1024, 768, 32);

			first = GetTickCount();
		}
		kEnterCriticalSection();
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());
		kLeaveCriticalSection();
	}

	return 0;
}

DWORD WINAPI ProcessRemoverProc(LPVOID parameter)
{
	Process* pProcess = (Process*)parameter;

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

		//Scheduler::GetInstance()->Yield(pProcess->GetProcessId());
		kLeaveCriticalSection();

		int second = GetTickCount();
		if (second - first >= 400)
		{
			first = GetTickCount();
		}
	}

	return 0;
}