#include "SkyConsole.h"
#include "SkyOS.h"
#include "ProcessManager.h"
#include "ZetPlane.h"
#include "RMEFunc.h"
#include "PCI.h"
#include "SystemProfiler.h"
#include "Process.h"
#include "Thread.h"
#include "SkyDebugger.h"
#include "SkyGUILauncher.h"
#include "lua.h"
#include "lualib.h"

long CmdCls(char *theCommand)
{
	SkyConsole::Clear();
	return false;
}

long CmdKill(char *theCommand)
{
	int id = atoi(theCommand);

	kEnterCriticalSection();

	Process* pProcess = ProcessManager::GetInstance()->FindProcess(id);

	kLeaveCriticalSection();

	if (pProcess != nullptr)
	{
		SkyConsole::Print("kill process : %s, ID : %d\n", pProcess->m_processName, pProcess->GetProcessId());
		ProcessManager::GetInstance()->RemoveProcess(pProcess->GetProcessId());
	}
	else
		SkyConsole::Print("process don't exist(%d)\n", id);	

	return false;
}

long CmdProcessList(char *theCommand)
{
	kEnterCriticalSection();
	SkyConsole::Print(" ID : Process Name\n");

	ProcessManager::ProcessList* processlist = ProcessManager::GetInstance()->GetProcessList();
	map<int, Process*>::iterator iter = processlist->begin();

	for (; iter != processlist->end(); ++iter)
	{
		Process* pProcess = (*iter).second;
		SkyConsole::Print("  %d %s\n", pProcess->GetProcessId(), pProcess->m_processName);
	}

	kLeaveCriticalSection();

	return true;
}

long cmdMemState(char *theCommand)
{
	SystemProfiler::GetInstance()->PrintMemoryState();
	return false;
}

long cmdCreateWatchdogTask(char* pName)
{
	kEnterCriticalSection();
	
	Process* pProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("WatchDog", WatchDogProc, NULL, PROCESS_KERNEL);	
	kLeaveCriticalSection();
	
	if(pProcess == nullptr)
		SkyConsole::Print("Can't create process\n");	

	return false;
}

long cmdTaskCount(char *theCommand)
{
	kEnterCriticalSection();

	ProcessManager::TaskList* taskList = ProcessManager::GetInstance()->GetTaskList();
	SkyConsole::Print("current task count %d\n", taskList->size());

	kLeaveCriticalSection();
	return false;
}

long cmdGlobalState(char *theCommand)
{
	SystemProfiler::GetInstance()->PrintGlobalState();
	return false;
}

long CmdExec(char *theCommand)
{

	Process* pProcess = ProcessManager::GetInstance()->CreateProcessFromFile(theCommand, nullptr, PROCESS_USER);

	if (pProcess == nullptr)
		SkyConsole::Print("Can't create process %s\n", theCommand);
	

	return false;
}
#include "SkyMockInterface.h"
#include "I_LuaModule.h"
extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef void(*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_LuaModule*(*PGetLuaModule)();

extern FILE* g_stdOut;
extern FILE* g_stdIn;
extern FILE* g_stdErr;

I_LuaModule* pLuaModule = nullptr;
long cmdLua(char *theCommand)
{
	g_printInterface.sky_stdin = g_stdIn;
	g_printInterface.sky_stdout = g_stdOut;
	g_printInterface.sky_stderr = g_stdErr;

	if (theCommand == nullptr)
		return false;

	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');

	if (pLuaModule != nullptr)
	{
		bool result = pLuaModule->DoFile(theCommand);

		if (result == false)
			SkyConsole::Print("Lua Exec Fail\n");

		return false;
	}
	
	MODULE_HANDLE hwnd = SkyModuleManager::GetInstance()->LoadModuleFromMemory("LUA3_DLL");

	if (hwnd == nullptr)
	{
		HaltSystem("LUA3_DLL Module Load Fail!!");
	}

	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "SetSkyMockInterface");
	PGetLuaModule GetLuaModuleInterface = (PGetLuaModule)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetLuaModule");

	//디버그 엔진에 플랫폼 종속적인 인터페이스를 넘긴다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);

	if (!GetLuaModuleInterface)
	{
		HaltSystem("Memory Module Load Fail!!");
	}

	pLuaModule = GetLuaModuleInterface();

	if (pLuaModule == nullptr)
	{
		HaltSystem("Lua Module Creation Fail!!");
	}


	pLuaModule->InitLua();	
	bool result = pLuaModule->DoFile(theCommand);
	//pLuaModule->CloseLua();

	if (result == false)
		SkyConsole::Print("Lua Exec Fail\n");

	return false;
}

long cmdGUI(char *theCommand)
{
	RequesGUIResolution();

	return false;
}

void FillRect8(int x, int y, int w, int h, char col, int actualX, int actualY)
{
	char* lfb = (char*)0xF0000000;

	for (int k = 0; k < h; k++)
		for (int j = 0; j < w; j++)
		{
			int index = ((j + x) + (k + y) * actualX);
			lfb[index] = col;
			index++;
		}

}

long cmdSwitchGUI(char *theCommand)
{
	//성공적으로 그래픽 모드가 전환되는지 확인
	if(true == SwitchGUIMode(1024, 768, 261))
	{
		//그래픽 버퍼 주소를 매핑해야 한다.
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), 0xE0000000, 0xE0000000, 0xE0FF0000);
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), 0xF0000000, 0xF0000000, 0xF0FF0000);
		
		//사각형을 그린다.
		FillRect8(100, 100, 100, 100, 8, 1024, 768);
		for (;;);
	}
	
	return false;
}


long cmdPCI(char *theCommand)
{
	RequestPCIList();

	return false;
}

long cmdCallStack(char *theCommand)
{
	SkyDebugger::GetInstance()->TraceStack();

	return false;
}

long cmdCallStack2(char *theCommand)
{
	SkyDebugger::GetInstance()->TraceStackWithSymbol();

	return false;
}

long cmdDir(char *theCommand)
{
	StorageManager::GetInstance()->GetFileList();

	return false;
}


