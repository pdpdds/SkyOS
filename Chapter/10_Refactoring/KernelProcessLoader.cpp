#include "KernelProcessLoader.h"
#include "Process.h"
#include "defines.h"
#include "HeapManager.h"
#include "fileio.h"
#include "StorageManager.h"

KernelProcessLoader::KernelProcessLoader()
{
}


KernelProcessLoader::~KernelProcessLoader()
{

}

Process* KernelProcessLoader::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param)
{
	Process* pProcess = new Process();
	pProcess->m_processId = GetNextProcessId();

	PageDirectory* pPageDirectory = VirtualMemoryManager::GetKernelPageDirectory();
	pProcess->SetPageDirectory(pPageDirectory);

	pProcess->m_dwRunState = TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);

	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;

	return pProcess;
}

Process* KernelProcessLoader::CreateProcessFromFile(char* appName, void* param)
{
	
	Process* pProcess = new Process();
	pProcess->m_processId = GetNextProcessId();

	PageDirectory* pPageDirectory = VirtualMemoryManager::GetKernelPageDirectory();
	pProcess->SetPageDirectory(pPageDirectory);
	
	pProcess->m_dwRunState = TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);


	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;

	return pProcess;
}
