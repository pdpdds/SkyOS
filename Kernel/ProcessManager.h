#pragma once
#include "VirtualMemoryManager.h"
#include "task.h"
#include "List.h"
#include "image.h"
#include "fsys.h"
#include "Process.h"
#include "Thread.h"

class Process;
class Thread;

#define PROC_INVALID_ID -1

class ProcessManager
{
public:
	ProcessManager();
	virtual ~ProcessManager();
		
	Process* GetCurrentProcess();	


	Orange::LinkedList* GetProcessList() { return &m_processList;}
	int GetNextProcessId() { return m_nextProcessId++; }

	static ProcessManager* GetInstance()
	{		
		if (m_processManager == nullptr)
			m_processManager = new ProcessManager();

		return m_processManager;
	}
	
	Process* CreateProcess(LPTHREAD_START_ROUTINE lpStartAddress, bool firstProcess = false);
	Process* CreateProcess(char* appName, UINT32 processType);	
	Thread* CreateThread(Process* pProcess, FILE* pFile, LPVOID param);
	Thread* CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);

	bool AddProcess(Process* pProcess);
	Process* FindProcess(int processId);
	bool RemoveFromTaskList(Process* pProcess);

	bool DestroyProcess(Process* pProcess);
	bool DestroyKernelProcess(Process* pProcess);
	bool ReleaseThreadContext(Process* pProcess);

	DoubleLinkedList* GetTaskList() { return &m_taskList; }

//Page Directory Mapping
	bool MapKernelSpace(PageDirectory* addressSpace);

private:	

private:
	static ProcessManager* m_processManager;

	int m_nextProcessId;	
	Orange::LinkedList m_processList;
	DoubleLinkedList m_taskList;
};