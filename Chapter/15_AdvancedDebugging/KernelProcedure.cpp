#include "SkyOS.h"
#include "SkyRenderer.h"
#include "nic.h"
#include "SkyGUISystem.h"
#include "SkyTest.h"
#include "SkyDebugger.h"

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

__declspec(naked) void ProcInit()
{
	__asm
	{		
		xor ebp, ebp; Set %ebp to NULL
		ret
	}
}

DWORD WINAPI SystemConsoleProc(LPVOID parameter)
{
	SkyConsole::Print("Console Mode Start!!\n");

	systemOn = true;
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	multiboot_info* pBootInfo = SkyModuleManager::GetInstance()->GetMultiBootInfo();
	StorageManager::GetInstance()->Initilaize(pBootInfo);
	kEnterCriticalSection();
	SkyDebugger::GetInstance()->LoadSymbol("DEBUG_ENGINE_DLL");
	kLeaveCriticalSection();

	NativeConsole();

	SkyConsole::Print("Bye!!");

	return 0;
}


DWORD WINAPI SystemGUIProc(LPVOID parameter)
{
	unsigned int* ebp = (unsigned int*)&parameter - 1;
	SkyConsole::Print("start ebp : %x\n", *ebp);	
	SkyConsole::Print("parameter : %x\n", parameter);

	systemOn = true;
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	multiboot_info* pBootInfo = SkyModuleManager::GetInstance()->GetMultiBootInfo();
	StorageManager::GetInstance()->Initilaize(pBootInfo);
	
	kEnterCriticalSection();
	SkyDebugger::GetInstance()->LoadSymbol("DEBUG_ENGINE_DLL");
	kLeaveCriticalSection();

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

void SampleFillRect(ULONG* lfb, int x, int y, int w, int h, int col)
{
	for (int k = 0; k < h; k++)
		for (int j = 0; j < w; j++)
		{
			int index = ((j + x) + (k + y) * 1024);
			lfb[index] = col;
		}
}

DWORD WINAPI GUIWatchDogProc(LPVOID parameter)
{
	Process* pProcess = (Process*)parameter;
	int pos = 0;

	int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF };
	int first = GetTickCount();
	
	//그래픽 버퍼 주소를 얻는다.
	ULONG* lfb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;

	//루프를 돌면서 오른쪽 상단에 사각형을 그린다.
	while (1)
	{
		int second = GetTickCount();
		//1초 단위로 색상을 변경한다.
		if (second - first >= TIMEOUT_PER_SECOND)
		{
			if (++pos > 2)
				pos = 0;

			if (m_bShowTSWatchdogClock)
			{
				SampleFillRect(lfb, 1004, 0, 20, 20, colorStatus[pos]);				
			}
				
			first = GetTickCount();
		}
		
		//빠르게 실행될 필요가 없으므로 실행시간을 타 프로세스에 양보한다.
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());		
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