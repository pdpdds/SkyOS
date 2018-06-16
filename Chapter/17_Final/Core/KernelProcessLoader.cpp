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
	//프로세스를 생성하고 프로세스 아이디를 할당한다.
	Process* pProcess = new Process();
	pProcess->SetProcessId(GetNextProcessId());
	
	PageDirectory* pPageDirectory = nullptr;

	//페이징 기능을 끄고 페이지 디렉토리를 생성한다.
	PhysicalMemoryManager::EnablePaging(false);

	pPageDirectory = VirtualMemoryManager::CreateCommonPageDirectory();

	if (pPageDirectory == nullptr)
	{
		PhysicalMemoryManager::EnablePaging(true);
		return nullptr;
	}

	//힙공간을 페이지 디렉토리에 매핑한다.
	HeapManager::MapHeapToAddressSpace(pPageDirectory);

	//그래픽 버퍼 주소를 페이지 디렉토리에 매핑한다.
	if (SkyGUISystem::GetInstance()->GUIEnable() == true)
	{	
		uintptr_t videoAddress = (uintptr_t)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(pPageDirectory, videoAddress, videoAddress, videoAddress + VIDEO_RAM_LOGICAL_ADDRESS_OFFSET);
	}
	
	//페이징을 활성화한다.
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
