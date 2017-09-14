#include "ConsoleManager.h"
#include "string.h"
#include "stdio.h"
#include "Console.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"
#include "task.h"
#include "ProcessManager.h"
#include "ZetPlane.h"
#include "PIT.h"

extern void enter_usermode();

ConsoleManager::ConsoleManager()
{
}


ConsoleManager::~ConsoleManager()
{
}

void cmd_alloc()
{
	for (int i = 0; i < 1000; i++)
	{
		ZetPlane* pPlane = new ZetPlane();
		pPlane->SetX(i);
		pPlane->SetY(i + 5);

		pPlane->IsRotate();

		SkyConsole::Print("Plane X : %d, Plane Y : %d\n", pPlane->GetX(), pPlane->GetY());		

		delete pPlane;
	}
}

/* video mode info. */
#define WIDTH           800
#define HEIGHT          600
#define BPP             32
#define BYTES_PER_PIXEL 4

/* BGA LFB is at LFB_PHYSICAL. */
#define LFB_PHYSICAL 0xE0000000
#define LFB_VIRTUAL  0x300000

/* render rectangle in 32 bpp modes. */
void rect32(int x, int y, int w, int h, int col) {
	uint32_t* lfb = (uint32_t*)LFB_VIRTUAL;
	for (uint32_t k = 0; k < h; k++)
		for (uint32_t j = 0; j < w; j++)
			lfb[(j + x) + (k + y) * WIDTH] = col;
}

/* thread cycles through colors of red. */
DWORD WINAPI kthread_1(LPVOID parameter) {
	int col = 0;
	bool dir = true;
	while (1) {
		rect32(200, 250, 100, 100, col << 16);
		if (dir) {
			if (col++ == 0xfe)
				dir = false;
		}
		else
			if (col-- == 1)
				dir = true;
	}

	return 0;
}

DWORD WINAPI SampleLoop2(LPVOID parameter) 
{
	char* str = "Hello world3!";

	int first = GetTickCount();
	while (1)
	{

		int second = GetTickCount();
		if (second - first > 100)
		{
			SkyConsole::Print("%s\n", str);

			first = GetTickCount();
		}


	}

	for (;;);
	return 0;
}

void cmd_memtask()
{
	EnterCriticalSection();

	Process* pProcess = ProcessManager::GetInstance()->CreateProcessFromMemory(SampleLoop2);
	
	LeaveCriticalSection();
}

void cmd_killtask(int processId)
{
	EnterCriticalSection();

	Process* pProcess = ProcessManager::GetInstance()->FindProcess(processId);

	if (pProcess != NULL)
	{
		SkyConsole::Print("Destroy Kernel Process : %s, ID : %d\n", pProcess->m_processName, pProcess->m_processId);
		ProcessManager::GetInstance()->DestroyKernelProcess(pProcess);
	}

	LeaveCriticalSection();
}



void cmd_list() {

	EnterCriticalSection();
	SkyConsole::Print("Process List\n");

	Orange::LinkedList* processlist = ProcessManager::GetInstance()->GetProcessList();

	for (int i = 0; i < processlist->Count(); i++)
	{
		Process* pProcess = (Process*)processlist->Get(i);

		SkyConsole::Print("Process Name : %s, ID : %d\n", pProcess->m_processName, pProcess->m_processId);
	}

	LeaveCriticalSection();
}


void cmd_memstate() {
		
	SkyConsole::Print("free block count %d\n", PhysicalMemoryManager::GetFreeBlockCount());
	SkyConsole::Print("total block count %d\n", PhysicalMemoryManager::GetTotalBlockCount());
	SkyConsole::Print("\n");	
}


//! read command
void cmd_read() {

	//! get pathname
	char path[32];
	SkyConsole::Print("ex: \"file.txt\", \"a:\\file.txt\", \"a:\\folder\\file.txt\"\n\rFilename> ");
	SkyConsole::GetCommand(path, 30);

	//! open file
	FILE file = volOpenFile(path);

	//! test for invalid file
	if (file.flags == FS_INVALID) {

		SkyConsole::Print("Unable to open file\n\r");
		return;
	}

	//! cant display directories
	if ((file.flags & FS_DIRECTORY) == FS_DIRECTORY) {

		SkyConsole::Print("Unable to display contents of directory.\n\r");
		return;
	}

	//! top line
	SkyConsole::Print("\n\n\r-------[%s]-------\n\r", file.name);

	//! display file contents
	while (file.eof != 1) {

		//! read cluster
		unsigned char buf[512];
		volReadFile(&file, buf, 512);

		//! display file
		for (int i = 0; i<512; i++)
			SkyConsole::WriteChar(buf[i]);

		//! wait for input to continue if not EOF
		if (file.eof != 1) {
			SkyConsole::Print("\n\r------[Press a key to continue]------");
			SkyConsole::GetChar();
			SkyConsole::Print("\r"); //clear last line
		}
	}

	//! done :)
	SkyConsole::Print("\n\n\r--------[EOF]--------");
}

void go_user() {

	int stack = 0;
	_asm mov[stack], esp

	extern void tss_set_stack(uint16_t, uint16_t);
	tss_set_stack(0x10, (uint16_t)stack & 0xffff);

	enter_usermode();

	char* testStr = "\n\rWe are inside of your computer...";

	//! call OS-print message
	_asm xor eax, eax
	_asm lea ebx, [testStr]
		_asm int 0x80

	//! cant do CLI+HLT here, so loop instead
	while (1);
}

// proc (process) command
void cmd_proc(char* pName) {

	int ret = 0;	
	if (pName == NULL)
		return;

	Process* pProcess = ProcessManager::GetInstance()->CreateProcessFromFile(pName, PROCESS_KERNEL);
	if (pProcess == 0)
	{
		SkyConsole::Print("Can't Execute Process. %d\n", pName);
	}
	else
		ProcessManager::GetInstance()->AddProcess(pProcess);
}

#include "flpydsk.h"
bool ConsoleManager::RunCommand(char* buf) 
{

	if (buf[0] == '\0')
		return false;	

	if (strcmp(buf, "user") == 0) {
		go_user();
	}

	//! exit command
	if (strcmp(buf, "exit") == 0) {
		return true;
	}

	//! clear screen
	else if (strcmp(buf, "cls") == 0) {
		SkyConsole::Clear();
	}

	//! help
	else if (strcmp(buf, "help") == 0) {

		SkyConsole::Print("Orange OS Console Help\n");		
		SkyConsole::Print(" - exit: quits and halts the system\n");
		SkyConsole::Print(" - list: displays process list\n");
		SkyConsole::Print(" - cls: clears the display\n");
		SkyConsole::Print(" - help: displays this message\n");
		SkyConsole::Print(" - read: reads a file\n");
		SkyConsole::Print(" - reset: Resets and recalibrates floppy for reading\n");
		SkyConsole::Print(" - proc: Run process\n");		
	}

	//! read sector
	else if (strcmp(buf, "read") == 0) {
		cmd_read();
		
		/*uint32_t sectornum = 0;
		char sectornumbuf[4];
		uint8_t* sector = 0;
		
		sectornum = 1;

		//DebugSkyConsole::Print("\n\rSector %i contents:\n\n\r", sectornum);

		//! read sector from disk
		sector = flpydsk_read_sector(sectornum);

		//! display sector
		if (sector != 0) {

			int i = 0;
			for (int c = 0; c < 1; c++) {

				for (int j = 0; j < 128; j++)									
					SkyConsole::Print("0x%x ", sector[i + j]);
				i += 128;

				SkyConsole::Print("Press any key to continue\n");				
			}
		}
		else
			SkyConsole::Print("*** Error reading sector from disk\n");

		SkyConsole::Print("Done.\n");*/



	}
	else if (strcmp(buf, "memstate") == 0) {
		cmd_memstate();
	}
	else if (strcmp(buf, "alloc") == 0) {
		cmd_alloc();
	}
	else if (strcmp(buf, "memtask") == 0) {
		cmd_memtask();
	}
	else if (strcmp(buf, "list") == 0) {
		cmd_list();
	}
	//! run process
	else if (strstr(buf, ".exe") > 0) {

		cmd_proc(buf);
	}	
	else
	{			
		char* directive = strtok(buf, " ");		

		if (directive != NULL && strcmp(directive, "kill") == 0)
		{
			char* processId = strtok(NULL, " ");

			if(processId == NULL)
				SkyConsole::Print("Argument insufficient\n", processId);
			else
			{
				int id = atoi(processId);
				cmd_killtask(id);
			}
		}
		else
			SkyConsole::Print("Unknown Command\n");		
	}

	return false;
}

