#include "SkyOS.h"
#include "KernelProcessLoader.h"

KernelProcessLoader::KernelProcessLoader()
{
}


KernelProcessLoader::~KernelProcessLoader()
{

}

Process* KernelProcessLoader::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param)
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

	if (SkyGUISystem::GetInstance()->GUIEnable() == true)
	{	
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(pPageDirectory, (uintptr_t)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr, (uintptr_t)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr, 
			(uintptr_t)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr + VIDEO_RAM_LOGICAL_ADDRESS_OFFSET);
	}
	

	PhysicalMemoryManager::EnablePaging(true);

	pProcess->SetPageDirectory(pPageDirectory);

	pProcess->m_dwRunState = TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);

	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;

	return pProcess;
}

Process* KernelProcessLoader::CreateProcessFromFile(char* appName, void* param)
{
	//Not Implemented

	return nullptr;
}
