#include "SkyConsole.h"
#include "SkyOS.h"
#include "ProcessManager.h"
#include "ZetPlane.h"
#include "PCI.h"
#include "SystemProfiler.h"
#include "Process.h"
#include "Thread.h"

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

long cmdPCI(char *theCommand)
{
	RequestPCIList();

	return false;
}

long cmdDir(char *theCommand)
{
	StorageManager::GetInstance()->GetFileList();

	return false;
}


