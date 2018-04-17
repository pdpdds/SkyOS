#include "ProcessManager.h"
#include "PhysicalMemoryManager.h"
#include "HeapManager.h"
#include "StorageManager.h"
#include "Process.h"
#include "Thread.h"
#include "Hal.h"
#include "string.h"
#include "memory.h"
#include "SkyConsole.h"
#include "ProcessUtil.h"
#include "SkyAPI.h"
#include "sysapi.h"
#include "KernelProcessLoader.h"
#include "UserProcessLoader.h"

ProcessManager* ProcessManager::m_processManager = nullptr;
static int kernelStackIndex = 0;

#define TASK_GENESIS_ID 1000

ProcessManager::ProcessManager()
{
	m_nextThreadId = 1000;

	m_pKernelProcessLoader = new KernelProcessLoader();
	m_pUserProcessLoader = new UserProcessLoader();;
}

ProcessManager::~ProcessManager()
{
}

Thread* ProcessManager::CreateThread(Process* pProcess, FILE* file, LPVOID param)
{
	unsigned char buf[512];
	IMAGE_DOS_HEADER* dosHeader = 0;
	IMAGE_NT_HEADERS* ntHeaders = 0;
	unsigned char* memory = 0;

	//파일에서 512바이트를 읽고 유효한 PE 파일인지 검증한다.
	int readCnt = StorageManager::GetInstance()->ReadFile(file, buf, 512, 1);

	if (0 == readCnt)
		return nullptr;

	if (!ValidatePEImage(buf)) 
	{
		SkyConsole::Print("Invalid PE Format!! %s\n", pProcess->m_processName);
		StorageManager::GetInstance()->CloseFile(file);
		return nullptr;
	}

	SkyConsole::Print("Valid PE Format %s\n", pProcess->m_processName);

	dosHeader = (IMAGE_DOS_HEADER*)buf;
	ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)buf);

	Thread* pThread = new Thread();
	pThread->m_pParent = pProcess;
	pProcess->m_imageBase = ntHeaders->OptionalHeader.ImageBase;
	pProcess->m_imageSize = ntHeaders->OptionalHeader.SizeOfImage;
	pThread->m_imageBase = ntHeaders->OptionalHeader.ImageBase;
	pThread->m_imageSize = ntHeaders->OptionalHeader.SizeOfImage;	
	pThread->m_dwPriority = 1;
	pThread->m_taskState = TASK_STATE_INIT;
	pThread->m_initialStack = 0;
	pThread->m_stackLimit = PAGE_SIZE;
	pThread->m_threadId = m_nextThreadId++;

	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)ntHeaders->OptionalHeader.AddressOfEntryPoint + ntHeaders->OptionalHeader.ImageBase;
	pThread->frame.flags = 0x200;


#ifdef _DEBUG
	SkyConsole::Print("Process ImageBase : 0x%X\n", ntHeaders->OptionalHeader.ImageBase);
#endif	

	int pageRest = 0;

	if ((pThread->m_imageSize % 4096) > 0)
		pageRest = 1;

	pProcess->m_dwPageCount = (pThread->m_imageSize / 4096) + pageRest;

	memory = (unsigned char*)PhysicalMemoryManager::AllocBlocks(pProcess->m_dwPageCount);

	/* map page into address space */
	for (DWORD i = 0; i < pProcess->m_dwPageCount; i++)
	{
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->GetPageDirectory(),
			ntHeaders->OptionalHeader.ImageBase + i * PAGE_SIZE,
			(uint32_t)memory + i * PAGE_SIZE,
			I86_PTE_PRESENT | I86_PTE_WRITABLE);
	}
	memset(memory, 0, pThread->m_imageSize);
	memcpy(memory, buf, 512);

	/* load image into memory */

	int fileRest = 0;
	if ((pThread->m_imageSize % 512) != 0)
		fileRest = 1;

	int readCount = (pThread->m_imageSize / 512) + fileRest;
	for (int i = 1; i < readCount; i++) 
	{
		if (file->_eof == 1)
			break;

		readCnt = StorageManager::GetInstance()->ReadFile(file, memory + 512 * i, 512, 1);		
	}

	StorageManager::GetInstance()->CloseFile(file);


	//스택을 생성하고 주소공간에 매핑한다.
	void* stackVirtual = (void*)(USER_VIRTUAL_STACK_ADDRESS + PAGE_SIZE * pProcess->m_stackIndex++);
	void* stackPhys = (void*)PhysicalMemoryManager::AllocBlock();

	SkyConsole::Print("Virtual Stack : %x\n", stackVirtual);
	SkyConsole::Print("Physical Stack : %x\n", stackPhys);

	/* map user process stack space */
	VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->GetPageDirectory(), (uint32_t)stackVirtual, (uint32_t)stackPhys, I86_PTE_PRESENT | I86_PTE_WRITABLE);

	/* final initialization */
	pThread->m_initialStack = (void*)((uint32_t)stackVirtual + PAGE_SIZE);
	pThread->frame.esp = (uint32_t)pThread->m_initialStack;
	pThread->frame.ebp = pThread->frame.esp;

	pThread->frame.eax = 0;
	pThread->frame.ecx = 0;
	pThread->frame.edx = 0;
	pThread->frame.ebx = 0;
	pThread->frame.esi = 0;
	pThread->frame.edi = 0;

	return pThread;
}

Thread* ProcessManager::CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param)
{
	Thread* pThread = new Thread();
	pThread->m_pParent = pProcess;
	pThread->m_threadId = m_nextThreadId++;
	pThread->m_dwPriority = 1;
	pThread->m_taskState = TASK_STATE_INIT;
	pThread->m_waitingTime = TASK_RUNNING_TIME;
	pThread->m_stackLimit = PAGE_SIZE;
	pThread->m_imageBase = 0;
	pThread->m_imageSize = 0;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)lpStartAddress;
	pThread->frame.flags = 0x200;
	pThread->m_startParam = param;

	//스택을 생성하고 주소공간에 매핑한다.
	void* stackVirtual = (void*)(KERNEL_VIRTUAL_STACK_ADDRESS + PAGE_SIZE * kernelStackIndex++);
	//void* stackVirtual = (void*)(KERNEL_VIRTUAL_STACK_ADDRESS + PAGE_SIZE * pProcess->m_kernelStackIndex++);
	void* stackPhys = (void*)PhysicalMemoryManager::AllocBlock();

#ifdef _DEBUG
	SkyConsole::Print("Virtual Stack : %x\n", stackVirtual);
	SkyConsole::Print("Physical Stack : %x\n", stackPhys);
#endif

	SkyConsole::Print("Physical Stack : %x\n", stackPhys);

	/* map user process stack space */
	VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->GetPageDirectory(), (uint32_t)stackVirtual, (uint32_t)stackPhys, I86_PTE_PRESENT | I86_PTE_WRITABLE);

	pThread->m_initialStack = (void*)((uint32_t)stackVirtual + PAGE_SIZE);
	pThread->frame.esp = (uint32_t)pThread->m_initialStack;
	pThread->frame.ebp = pThread->frame.esp;
		
	return pThread;
}

Process* ProcessManager::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType)
{	
	Process* pProcess = nullptr;
	if (processType == PROCESS_KERNEL)
		pProcess = m_pKernelProcessLoader->CreateProcessFromMemory(appName, lpStartAddress, param);
	else
		pProcess = m_pUserProcessLoader->CreateProcessFromMemory(appName, lpStartAddress, param);

	if (pProcess == nullptr)
		return nullptr;

	if (param == nullptr)
		param = pProcess;

	Thread* pThread = CreateThread(pProcess, lpStartAddress, param);

	M_Assert(pThread != nullptr, "MainThread is null.");

	bool result = pProcess->AddMainThread(pThread);

	M_Assert(result == true, "AddMainThread Method Failed.");

	result = AddProcess(pProcess);

	M_Assert(result == true, "AddProcess Method Failed.");

#ifdef _SKY_DEBUG
	SkyConsole::Print("Process Created. Process Id : %d\n", pProcess->m_processId);
#endif

	return pProcess;
}

Process* ProcessManager::CreateProcessFromFile(char* appName, void* param, UINT32 processType)
{
	FILE* file;

	file = StorageManager::GetInstance()->OpenFile(appName, "r");

	if (file == nullptr || file->_flags == FS_INVALID)
		return nullptr;

	if ((file->_flags & FS_DIRECTORY) == FS_DIRECTORY)
		return nullptr;

	Process* pProcess = nullptr;
	if (processType == PROCESS_KERNEL)
		pProcess = m_pKernelProcessLoader->CreateProcessFromFile(appName, param);
	else
		pProcess = m_pUserProcessLoader->CreateProcessFromFile(appName, param);

	if (pProcess == nullptr)
	{
		StorageManager::GetInstance()->CloseFile(file);
		return nullptr;
	}

	Thread* pThread = CreateThread(pProcess, file, NULL);

	M_Assert(pThread != nullptr, "MainThread is null.");

	bool result = pProcess->AddMainThread(pThread);

	M_Assert(result == true, "AddMainThread Method Failed.");

	result = AddProcess(pProcess);

	M_Assert(result == true, "AddProcess Method Failed.");

#ifdef _SKY_DEBUG
	SkyConsole::Print("Process Created. Process Id : %d\n", pProcess->m_processId);
#endif

	return pProcess;
}

Process* ProcessManager::FindProcess(int processId)
{
	ProcessList::iterator iter = m_processList.find(processId);

	if (iter == m_processList.end())
		return nullptr;

	return iter->second;
}

bool ProcessManager::AddProcess(Process* pProcess)
{
	if (pProcess == nullptr)
		return false;

	int entryPoint = 0;
	unsigned int procStack = 0;

	if (pProcess->m_processId == PROC_INVALID_ID)
		return false;

	if (!pProcess->GetPageDirectory())
		return false;

	Thread* pThread = pProcess->GetMainThread();

#ifdef _DEBUG
	// 메인스레드의 EIP, ESP를 얻는다
	entryPoint = pThread->frame.eip;
	procStack = pThread->frame.esp;

	SkyConsole::Print("eip : %x\n", pThread->frame.eip);
	SkyConsole::Print("page directory : %x\n", pProcess->GetPageDirectory());
	SkyConsole::Print("procStack : %x\n", procStack);
#endif	

	kEnterCriticalSection();
	m_processList.insert(std::make_pair(pProcess->m_processId, pProcess));

	m_taskList.push_back(pThread);

	kLeaveCriticalSection();

	return true;
}

Process* ProcessManager::GetCurrentProcess()
{
	if (m_taskList.empty())
		return nullptr;

	Thread* pThread = m_taskList.front();

	if (pThread == nullptr)
		return nullptr;

	return pThread->m_pParent;
}

bool ProcessManager::RemoveFromTaskList(Process* pProcess)
{
	Process::ThreadList::iterator iter = pProcess->m_threadList.begin();
	TaskList tempThreadList;

	for (; iter != pProcess->m_threadList.end(); iter++)
	{
		tempThreadList.push_back(iter->second);
	}

	tempThreadList.clear();
		
	return true;
}