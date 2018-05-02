#include "UserProcessLoader.h"
#include "SkyOS.h"

UserProcessLoader::UserProcessLoader()
{
}


UserProcessLoader::~UserProcessLoader()
{
}

Process* UserProcessLoader::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param)
{
	//Not Implemented
	return nullptr;
}
Process* UserProcessLoader::CreateProcessFromFile(char* appName, void* param)
{

	Process* pProcess = new Process();
	pProcess->SetProcessId(GetNextProcessId());
	PageDirectory* pPageDirectory = nullptr;

	//if (strcmp(appName, "ConsoleSystem") != 0)
	//{

	PhysicalMemoryManager::EnablePaging(false);

	pPageDirectory = VirtualMemoryManager::CreateCommonPageDirectory();

	if (pPageDirectory == nullptr)
	{
		PhysicalMemoryManager::EnablePaging(true);
		return nullptr;

	}

	HeapManager::MapHeapToAddressSpace(pPageDirectory);

	PhysicalMemoryManager::EnablePaging(true);

	//}
	//else
	//pPageDirectory = VirtualMemoryManager::GetKernelPageDirectory();	

	pProcess->SetPageDirectory(pPageDirectory);

	pProcess->m_dwRunState = TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);

	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;

	return pProcess;
}
