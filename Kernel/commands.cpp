#include "SkyConsole.h"
#include "string.h"
#include "stdio.h"
#include "Hal.h"
#include "ProcessManager.h"
#include "ZetPlane.h"
#include "PhysicalMemoryManager.h"
#include "PIT.h"
#include "KernelProcedure.h"
#include "Graphics.h"
#include "SkyAPI.h"
#include "HariboteGUI.h"

long cmdProcessList(char *theCommand)
{

	kEnterCriticalSection(&g_criticalSection);
	SkyConsole::Print(" ID : Process Name\n");

	Sky::LinkedList* processlist = ProcessManager::GetInstance()->GetProcessList();

	for (int i = 0; i < processlist->Count(); i++)
	{
		Process* pProcess = (Process*)processlist->Get(i);

		SkyConsole::Print("  %d : %s\n", pProcess->m_processId, pProcess->m_processName);
	}

	kLeaveCriticalSection(&g_criticalSection);

	return false;
}

long cmdMemState(char *theCommand)
{
	SkyConsole::Print("free block count %d\n", PhysicalMemoryManager::GetFreeBlockCount());
	SkyConsole::Print("total block count %d\n", PhysicalMemoryManager::GetTotalBlockCount());
	SkyConsole::Print("\n");
	return false;
}

long cmdTestCPlusPlus(char *theCommand)
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

	return false;
}

long clsCmd(char *theCommand)
{
	SkyConsole::Clear();
	return false;
}

long cmdKillTask(char *theCommand)
{
	if (theCommand == NULL)
	{
		SkyConsole::Print("Argument insufficient\n");
		return false;
	}

	int id = atoi(theCommand);


	kEnterCriticalSection(&g_criticalSection);

	Process* pProcess = ProcessManager::GetInstance()->FindProcess(id);

	if (pProcess != NULL)
	{
		SkyConsole::Print("kill process : %s, ID : %d\n", pProcess->m_processName, pProcess->m_processId);
		ProcessManager::GetInstance()->DestroyKernelProcess(pProcess);
	}
	else
		SkyConsole::Print("process don't exist(%d)\n", id);

	kLeaveCriticalSection(&g_criticalSection);
	return false;
}

long cmdRead(char *theCommand)
{
	if (theCommand == NULL || strlen(theCommand) == 0)
	{
		SkyConsole::Print("ex: \"file.txt\", \"a:\\file.txt\", \"a:\\folder\\file.txt\"\n");
		return false;
	}

	FILE file = volOpenFile(theCommand);

	//! test for invalid file
	if (file.flags == FS_INVALID) {

		SkyConsole::Print("Unable to open (%s) file\n", theCommand);
		return false;
	}

	//! cant display directories
	if ((file.flags & FS_DIRECTORY) == FS_DIRECTORY) {

		SkyConsole::Print("Unable to display contents of directory.\n");
		return false;
	}

	//! top line
	SkyConsole::Print("\n-------[%s]-------\n", file.name);

	//! display file contents
	while (file.eof != 1) {

		//! read cluster
		unsigned char buf[512];
		volReadFile(&file, buf, 512);

		//! display file
		for (int i = 0; i < 512; i++)
			SkyConsole::WriteChar(buf[i]);

		//! wait for input to continue if not EOF
		if (file.eof != 1) {
			SkyConsole::Print("\n------[Press a key to continue]------");
			SkyConsole::GetChar();
		}
	}

	//! done :)
	SkyConsole::Print("\n\n--------[EOF]--------\n");
	return false;
}

long cmdProc(char* pName) {

	int ret = 0;
	if (pName == NULL)
		return false;

	Process* pProcess = ProcessManager::GetInstance()->CreateProcessFromFile(pName, PROCESS_KERNEL);
	if (pProcess == 0)
	{
		SkyConsole::Print("Can't Execute Process. %d\n", pName);
	}
	else
	{
		pProcess->m_IskernelProcess = false;
		ProcessManager::GetInstance()->AddProcess(pProcess);
	}

	return false;
}

long cmdTesttask(char* pName)
{
	kEnterCriticalSection(&g_criticalSection);

	Process* pProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("TestProc", TestProc, NULL);

	if (pProcess == 0)
	{
		SkyConsole::Print("Can't Execute Process. %d\n", pName);
	}


	kLeaveCriticalSection(&g_criticalSection);

	return false;
}


extern void Beep();


long cmdBeep(char* pName)
{
	Beep();

	return false;
}

extern void TestV8086();
extern uint8_t * bochs_vid_memory;


char hankaku[4096];

bool LoadFont(char* fontName)
{
	FILE file = volOpenFile(fontName);
	memset(hankaku, 0, 4096);

	//! test for invalid file
	if (file.flags == FS_INVALID) {

		SkyConsole::Print("Unable to open (%s) file\n", fontName);
		return false;
	}

	//! cant display directories
	if ((file.flags & FS_DIRECTORY) == FS_DIRECTORY) {

		SkyConsole::Print("Unable to display contents of directory.\n");
		return false;
	}

	unsigned char* buffer = (unsigned char*)hankaku;
	int bufferIndex = 0;
	int charIndex = 0;

	memset(buffer, 0, 4096);

	while (file.eof != 1) {

		char buf[512];
		volReadFile(&file, (unsigned char*)buf, 512);

		for (int i = 0; i < 512; i++)
		{
			if (buf[i] == '*')
			{

				if (charIndex < 8)
				{
					char a = (char)(1 << (8 - 1 - charIndex));
					buffer[bufferIndex] |= a;
				}
				


			}

			if ((buf[i] == '*') || (buf[i] == '.'))
			{
				charIndex++;

				if (charIndex >= 8)
				{
					bufferIndex++;
					charIndex = 0;					
				}
			}
		}

		if (bufferIndex > 2000)
			break;

	}

	return true;
}

#include "FontData.h"
bool LoadFontFromMemory()
{	
	unsigned char* buffer = (unsigned char*)hankaku;
	int bufferIndex = 0;
	int charIndex = 0;

	memset(buffer, 0, 4096);

	int readIndex = 0;

	while (readIndex < 32768) {

		
		for (int i = 0; i < 512; i++)
		{
			if (fontData[readIndex + i] == '*')
			{

				if (charIndex < 8)
				{
					char a = (char)(1 << (8 - 1 - charIndex));
					buffer[bufferIndex] |= a;
				}



			}

			if ((fontData[readIndex + i] == '*') || (fontData[readIndex + i] == '.'))
			{
				charIndex++;

				if (charIndex >= 8)
				{
					bufferIndex++;
					charIndex = 0;
				}
			}
		}

		readIndex += 512;

	}

	return true;
}

#include "desq.h"
#include "svgaguientry.h"
long cmdGUI(char *theCommand)
{
	bool result = graphics_install_vesa(1024, 768, 8);
	//bool result = graphics_install_vesa(1024, 768, 24);
	//run_svgagui();

	StartSampleGui();

	/*if (result == true)
	{
		//LoadFont("hankaku.txt");
		LoadFontFromMemory();

		HariboteGUI* pGUIManager = new HariboteGUI();
		//pGUIManager->TestRun(bochs_vid_memory);
		pGUIManager->Init((char*)bochs_vid_memory, 1024, 768);
		pGUIManager->Run();
	}*/

	return false;
}