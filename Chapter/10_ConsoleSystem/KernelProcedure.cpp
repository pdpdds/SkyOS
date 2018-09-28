#include "SkyOS.h"
#include "nic.h"
#include "SkyTest.h"
#include "KeyboardController.h"

bool systemOn = false;

void NativeConsole()
{
	ConsoleManager manager;

	char	commandBuffer[MAXPATH];
	//루프를 돌면서 커맨드를 입력받는다.
	while (1)
	{
		SkyConsole::Print("Command> ");
		memset(commandBuffer, 0, MAXPATH);

		//버퍼로 부터 커맨드를 얻어온다.
		SkyConsole::GetCommand(commandBuffer, MAXPATH - 2);
		SkyConsole::Print("\n");

		//커맨드를 파싱한 후 실행한다.
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

	//콘솔용 키보드 초기화
	KeyboardController::SetupInterrupts();
	SkyConsole::Print("Keyboard Init..\n");

	systemOn = true;
	//타이머 활성화
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	//저장장치 초기화
	multiboot_info* pBootInfo = SkyModuleManager::GetInstance()->GetMultiBootInfo();
	StorageManager::GetInstance()->Initilaize(pBootInfo);

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