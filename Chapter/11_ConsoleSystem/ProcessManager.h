#pragma once
#include "VirtualMemoryManager.h"
#include "SkyStruct.h"
#include "ProcessUtil.h"
#include "List.h"
#include "PEImage.h"
#include "Process.h"
#include "Thread.h"
#include "fileio.h"

class Process;
class Thread;
class Loader;

class ProcessManager
{

public:
	ProcessManager();
	virtual ~ProcessManager();

	typedef std::map<int, Process*> ProcessList;
	typedef std::list<Thread*> TaskList;
		
	Process* GetCurrentProcess();	

	ProcessList* GetProcessList() { return &m_processList;}


	static ProcessManager* GetInstance()
	{		
		if (m_processManager == nullptr)
			m_processManager = new ProcessManager();

		return m_processManager;
	}
		
	Process* CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType);
	Process* CreateProcessFromFile(char* appName, void* param, UINT32 processType);

	Thread* CreateThread(Process* pProcess, FILE* pFile, LPVOID param);
	Thread* CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);

	Process* FindProcess(int processId);
	bool RemoveFromTaskList(Process* pProcess);
	
	TaskList* GetTaskList() { return &m_taskList; }

private:
	bool AddProcess(Process* pProcess);

private:
	static ProcessManager* m_processManager;
	int m_nextThreadId;

	Loader* m_pKernelProcessLoader;
	Loader* m_pUserProcessLoader;

	ProcessList m_processList;
	TaskList m_taskList;
	TaskList m_terminatedTaskList;
};