#include "KernelProcessLoader.h"
#include "Process.h"
#include "defines.h"
#include "HeapManager.h"
#include "fileio.h"
#include "StorageManager.h"
#include "SkyConsole.h"
#include "VideoRam.h"

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

#ifdef SKY_GUI
	VideoRamInfo ramInfo = VideoRam::GetInstance()->GetVideoRamInfo();
	VirtualMemoryManager::CreateVideoDMAVirtualAddress(pPageDirectory, VIDEO_RAM_LOGICAL_ADDRESS, (uintptr_t)ramInfo._pVideoRamPtr, 0xFE000000);
#endif

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
