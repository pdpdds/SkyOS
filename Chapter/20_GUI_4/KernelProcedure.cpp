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
#include "SkyGUI.h"
#include "ConsoleIOListener.h"
#include "SkyRenderer.h"
#include "sprintf.h"
#include "SkySheet.h"

extern bool systemOn;

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


	//SkyWindow* pWindow = new SkySimpleGUI();
	SkyWindow* pWindow = new SkyGUI();
	VideoRamInfo& info = VideoRam::GetInstance()->GetVideoRamInfo();

	pWindow->Initialize(info._pVideoRamPtr, info._width, info._height, info._bpp);

	pWindow->Run();

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

int cons_newline(int cursor_y, SkySheet *sheet)
{
	int x, y;
	unsigned char* buf = sheet->GetBuf();
	int bxsize = sheet->GetXSize();
	if (cursor_y < 28 + 112) {
		cursor_y += 16; /* 다음 행에 */
	}
	else {
		/* 스크롤 */
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 240; x++) {
				buf[x + y * bxsize] = buf[x + (y + 16) * bxsize];
			}
		}
		for (y = 28 + 112; y < 28 + 128; y++) {
			for (x = 8; x < 8 + 240; x++) {
				buf[x + y * bxsize] = COL8_000000;
			}
		}
		sheet->Refresh(8, 28, 8 + 240, 28 + 128);
	}
	return cursor_y;
}

DWORD WINAPI ConsoleGUIProc(LPVOID parameter)
{
	Thread* pThread = ProcessManager::GetInstance()->GetCurrentTask();
	Process* pProcess = pThread->m_pParent;

	SkyGUI* pGUI = (SkyGUI*)parameter;
	SkyRenderer* pRenderer = pGUI->GetRenderer();
	ConsoleIOListener* listener = new ConsoleIOListener();

	SkySheet *sheet = pGUI->FindSheetByID(pProcess->GetProcessId());

	pGUI->RegisterIOListener(pProcess->GetProcessId(), listener);

	char s[30], cmdline[30];
	int  cursor_x = 16, cursor_y = 28, cursor_c = -1;
	
	cursor_c = COL8_FFFFFF;

	/* prompt 표시 */
	pRenderer->PutFontAscToSheet(sheet, 8, 28, COL8_FFFFFF, COL8_000000, ">", 1);

	for (;;) {
		kEnterCriticalSection();
		if (listener->ReadyStatus() == false)
		{			
			Scheduler::GetInstance()->Yield(pProcess->GetProcessId());
			kLeaveCriticalSection();
			continue;
		}
		else
		{
			int i = listener->GetStatus();
			kLeaveCriticalSection();
			if (i <= 1) { /* 커서용 타이머 */
				if (i != 0) {
					//timer_init(timer, &task->fifo, 0); /* 다음은 0을 */
					if (cursor_c >= 0) {
						cursor_c = COL8_FFFFFF;
					}
				}
				else {
					//timer_init(timer, &task->fifo, 1); /* 다음은 1을 */
					if (cursor_c >= 0) {
						cursor_c = COL8_000000;
					}
				}
				//timer_settime(timer, 50);
			}
			if (i == 2) {	/* 커서 ON */
				cursor_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* 커서 OFF */
				pRenderer->BoxFill(sheet->GetBuf(), sheet->GetXSize(), COL8_000000, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
				cursor_c = -1;
			}
			if (256 <= i && i <= 511) { /* 키보드 데이터(태스크 A경유) */
				if (i == 8 + 256) {
					/* 백 스페이스 */
					if (cursor_x > 16) {
						/* 스페이스로 지우고 나서, 커서를 1개 back */
						pRenderer->PutFontAscToSheet(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
						cursor_x -= 8;
					}
				}
				else if (i == 10 + 256) {

					/* Enter */
					/* 커서를 스페이스에서 지우고 나서 개행한다 */
					pRenderer->PutFontAscToSheet(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
					cmdline[cursor_x / 8 - 2] = 0;
					cursor_y = cons_newline(cursor_y, sheet);

					if (strcmp(cmdline, "mem") == 0)
					{
						size_t totalMemory = PhysicalMemoryManager::GetMemorySize();
						sprintf(s, "total   %dMB", totalMemory / (1024 * 1024));
						pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						sprintf(s, "free %dKB", PhysicalMemoryManager::GetFreeMemory() / 1024);
						pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						cursor_y = cons_newline(cursor_y, sheet);
					}
					else if (strcmp(cmdline, "cls") == 0) /* cls 커맨드 */
					{
						
						unsigned char* buf = sheet->GetBuf();
						int bxsize = sheet->GetXSize();
						for (int y = 28; y < 28 + 128; y++) {
							for (int x = 8; x < 8 + 240; x++) {
								buf[x + y * bxsize] = COL8_000000;
							}
						}
						sheet->Refresh(8, 28, 8 + 240, 28 + 128);
						cursor_y = 28;
					}
					
					else if (cmdline[0] != 0) {
						/* 커멘드도 아니고 빈 행도 아니다 */
						pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "Bad command.", 12);
						cursor_y = cons_newline(cursor_y, sheet);
						cursor_y = cons_newline(cursor_y, sheet);
					}
					/* prompt 표시 */
					pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, ">", 1);
					cursor_x = 16;
				}
				else {

					/* 일반 문자 */
					if (cursor_x < 240) {
						/* 한 글자 표시하고 나서 커서를 1개 진행한다 */
						s[0] = i - 256;
						s[1] = 0;
						cmdline[cursor_x / 8 - 2] = i - 256;
						pRenderer->PutFontAscToSheet(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, s, 1);
						cursor_x += 8;
					}

				}
			}
			/* 커서재표시 */
			if (cursor_c >= 0) {
				pRenderer->BoxFill(sheet->GetBuf(), sheet->GetXSize(), cursor_c, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
			}
			sheet->Refresh(cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
		}
	}

	return 0;
}