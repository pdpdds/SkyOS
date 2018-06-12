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

	PhysicalMemoryManager::EnablePaging(false);

	pPageDirectory = VirtualMemoryManager::CreateCommonPageDirectory();

	if (pPageDirectory == nullptr)
	{
		PhysicalMemoryManager::EnablePaging(true);
		return nullptr;

	}

	HeapManager::MapHeapToAddressSpace(pPageDirectory);

	//그래픽 버퍼 주소를 페이지 디렉토리에 매핑한다.
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
