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
#include "sysapi.h"


ProcessManager* ProcessManager::m_processManager = nullptr;
static int kernelStackIndex = 0;

ProcessManager::ProcessManager()
{
	m_nextProcessId = 1;
	m_nextThreadId = 1000;
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
	for (int i = 0; i < pProcess->m_dwPageCount; i++)
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

/*Process* ProcessManager::CreateProcessFromFile(char* appName, UINT32 processType)
{
	FILE file;

	file = volOpenFile(appName);

	if (file.flags == FS_INVALID)
		return NULL;

	if ((file.flags & FS_DIRECTORY) == FS_DIRECTORY)
		return 0;


	PageDirectory* addressSpace = MapKernelSpace();

	if (!addressSpace)
	{
		volCloseFile(&file);
		return NULL;
	}

	VirtualMemoryManager::MapHeap(addressSpace);

	//MapSysAPIAddress(addressSpace);

	Process* pProcess = new Process();
	pProcess->m_processId = ProcessManager::GetInstance()->GetNextProcessId();
	pProcess->m_pPageDirectory = addressSpace;
	pProcess->m_dwPriority = 1;
	pProcess->m_dwRunState = TASK_STATE_INIT;
	pProcess->m_dwProcessType = processType;
	strcpy(pProcess->m_processName, appName);

	Thread* pThread = CreateThread(pProcess, &file, NULL);

	pProcess->AddMainThread(pThread);

	return pProcess;
}*/

Process* ProcessManager::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param)
{	
	int flags = I86_PTE_PRESENT | I86_PTE_WRITABLE;

	Process* pProcess = new Process();
	pProcess->m_processId = GetNextProcessId();

	PageDirectory* pPageDirectory = VirtualMemoryManager::CreateCommonPageDirectory();
	pProcess->SetPageDirectory(pPageDirectory);

	pProcess->m_dwRunState = TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);
	

	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;

	HeapManager::MapHeapToAddressSpace(pPageDirectory);

	Thread* pThread = CreateThread(pProcess, lpStartAddress, param);

	pProcess->AddMainThread(pThread);

	AddProcess(pProcess);

#ifdef _DEBUG
	SkyConsole::Print("Create Success Task %d\n", pProcess->m_processId);
#endif

	return pProcess;
}

Process* ProcessManager::CreateProcessFromFile(char* appName, UINT32 processType)
{
	FILE* file;
	
	file = StorageManager::GetInstance()->OpenFile(appName, "r");

	if (file == nullptr || file->_flags == FS_INVALID)
		return nullptr;

	if ((file->_flags & FS_DIRECTORY) == FS_DIRECTORY)
		return nullptr;

	PageDirectory* addressSpace = MapKernelSpace();

	if (!addressSpace)
	{
		StorageManager::GetInstance()->CloseFile(file);
		return nullptr;
	}

	Process* pProcess = new Process();
	pProcess->m_processId = GetNextProcessId();

	pProcess->SetPageDirectory(addressSpace);
	//PageDirectory* pPageDirectory = VirtualMemoryManager::CreatePageDirectory();
	//pProcess->SetPageDirectory(pPageDirectory);	
	pProcess->m_dwRunState = TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);

	/*PageTable* identityPageTable = (PageTable*)PhysicalMemoryManager::AllocBlock();
	if (identityPageTable == NULL)
		return false;

	//0-4MB 의 물리 주소를 가상 주소와 동일하게 매핑시킨다
	for (int i = 0, frame = 0x0, virt = 0x00000000; i < PAGES_PER_TABLE; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
	{
		PTE page = 0;
		PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);
		PageTableEntry::SetFrame(&page, frame);

		identityPageTable->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	PageDirectory* pPageDirectroy = pProcess->GetPageDirectory();
	PDE* identityEntry = (PDE*)&(pPageDirectroy->m_entries[PAGE_DIRECTORY_INDEX(0x00000000)]);	
	PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_PRESENT);
	PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_WRITABLE);
	PageDirectoryEntry::SetFrame(identityEntry, (uint32_t)identityPageTable);


	int flags = I86_PTE_PRESENT | I86_PTE_WRITABLE;

	//커널 이미지를 주소공간에 매핑. 커널 크기는 4메가가 넘지 않는다고 가정한다
	//1024 * PAGE_SIZE = 4MB
	uint32_t virtualAddr = KERNEL_PHYSICAL_BASE_ADDRESS;
	uint32_t physAddr = KERNEL_PHYSICAL_BASE_ADDRESS;

	for (uint32_t i = 0; i < PAGES_PER_TABLE; i++)
	{
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->GetPageDirectory(), virtualAddr + (i*PAGE_SIZE), physAddr + (i*PAGE_SIZE), flags);
	}
	*/
	VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->GetPageDirectory(), (uint32_t)pProcess->GetPageDirectory(), (uint32_t)pProcess->GetPageDirectory(), I86_PTE_PRESENT | I86_PTE_WRITABLE);
	
	//20161109	
	//pProcess->SetPageDirectory(VirtualMemoryManager::GetCurPageDirectory());

	HeapManager::MapHeapToAddressSpace(pProcess->GetPageDirectory());

	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;

	Thread* pThread = CreateThread(pProcess, file, NULL);

	if (pThread == nullptr)
	{
		HaltSystem("ProcessManager::CreateProcessFromFile, Create Thread Fail");
	}

	pProcess->AddMainThread(pThread);
	
#ifdef _DEBUG
	SkyConsole::Print("Create Success Task %d\n", pProcess->m_processId);
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

//firstProcess가 true일 경우 커널의 최초 프로세스를 생성한다.
//이 시점에서만 이전에 커널힙이 생성되었다고 가정한다.
//이후 프로세스는 여기서 힙을 별도로 생성한다.
Process* ProcessManager::CreateKernelProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param)
{
	Process* pProcess = new Process();

	if (pProcess == nullptr)
		HaltSystem("ProcessManager::CreateKernelProcessFromMemory\n");

	pProcess->m_processId = GetNextProcessId();

	PageDirectory* pPageDirectory = VirtualMemoryManager::GetCurPageDirectory();
	pProcess->SetPageDirectory(pPageDirectory);
	
	pProcess->m_dwRunState = TASK_STATE_RUNNING;
	strcpy(pProcess->m_processName, appName);

	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;

	if (param == nullptr)
		param = pProcess;

	Thread* pThread = CreateThread(pProcess, lpStartAddress, param);

	if (pThread == nullptr)
		HaltSystem("ProcessManager::CreateKernelProcessFromMemory\n");

	pProcess->AddMainThread(pThread);

	AddProcess(pProcess);

#ifdef _DEBUG
	SkyConsole::Print("Create Success Task %d\n", pProcess->m_processId);
#endif

	return pProcess;
}

//firstProcess가 true일 경우 커널의 최초 프로세스를 생성한다.
//이 시점에서만 이전에 커널힙이 생성되었다고 가정한다.
//이후 프로세스는 여기서 힙을 별도로 생성한다.
/*Process* ProcessManager::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress)
{
	Process* pProcess = new Process();
	pProcess->m_processId = GetNextProcessId();

	pProcess->m_pPageDirectory = VirtualMemoryManager::CreateAddressSpace();
	pProcess->m_dwRunState = TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);

	PageTable* identityPageTable = (PageTable*)PhysicalMemoryManager::AllocBlock();
	if (identityPageTable == NULL)
		return false;

	//0-4MB 의 물리 주소를 가상 주소와 동일하게 매핑시킨다
	for (int i = 0, frame = 0x0, virt = 0x00000000; i < PAGES_PER_TABLE; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
	{
		PTE page = 0;
		PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);
		PageTableEntry::SetFrame(&page, frame);

		identityPageTable->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	PDE* identityEntry = &pProcess->m_pPageDirectory->m_entries[PAGE_DIRECTORY_INDEX(0x00000000)];
	PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_PRESENT);
	PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_WRITABLE);
	PageDirectoryEntry::SetFrame(identityEntry, (uint32_t)identityPageTable);

	VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory, (uint32_t)pProcess->m_pPageDirectory, (uint32_t)pProcess->m_pPageDirectory, I86_PTE_PRESENT | I86_PTE_WRITABLE);
	VirtualMemoryManager::MapHeap(pProcess->m_pPageDirectory);


	int flags = I86_PTE_PRESENT | I86_PTE_WRITABLE;

	//커널 이미지를 주소공간에 매핑. 커널 크기는 4메가가 넘지 않는다고 가정한다
	//1024 * PAGE_SIZE = 4MB
	uint32_t virtualAddr = KERNEL_PHYSICAL_BASE_ADDRESS;
	uint32_t physAddr = KERNEL_PHYSICAL_BASE_ADDRESS;

	for (uint32_t i = 0; i < PAGES_PER_TABLE; i++)
	{
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory, virtualAddr + (i*PAGE_SIZE), physAddr + (i*PAGE_SIZE), flags);
	}


	pProcess->m_lpHeap = (void*)(KERNEL_VIRTUAL_HEAP_ADDRESS);

	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;

	Thread* pThread = CreateThread(pProcess, lpStartAddress, pProcess);


	pProcess->AddMainThread(pThread);

	AddProcess(pProcess);

#ifdef _DEBUG
	SkyConsole::Print("Create Success Task %d\n", pProcess->m_processId);
#endif

	return pProcess;
}*/

PageDirectory* ProcessManager::MapKernelSpace()
{
	return VirtualMemoryManager::CreateCommonPageDirectory();
}

/*void ProcessManager::MapSysAPIAddress(PageDirectory* dir)
{
	uint32_t virtualAddr = KERNEL_PHYSICAL_STACK_ADDRESS;
	uint32_t physAddr = uint32_t(&_syscalls[0]);

	for (uint32_t i = 0; i < PAGES_PER_TABLE; i++)
	{
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(dir, virtualAddr + (i*PAGE_SIZE), physAddr + (i*PAGE_SIZE), I86_PTE_PRESENT | I86_PTE_WRITABLE);
	}
}*/

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

/*bool ProcessManager::DestroyProcess(Process* pProcess)
{
	//태스크 목록에서 프로세스의 태스크들을 제거한다.
	RemoveFromTaskList(pProcess);

	//스레드 관련 Context 자원을 해제한다.
	//가상 주소를 운영하기 위해 할당했던 페이지 테이블, 스택 등을 회수 등등
	ReleaseThreadContext(pProcess);

	for (uint32_t page = 0; page < pProcess->m_dwPageCount; page++)
	{
		uint32_t virt = pProcess->m_imageBase + (page * PAGE_SIZE);

		VirtualMemoryManager::UnmapPhysicalAddress(pProcess->m_pPageDirectory, virt);
	}

// 힙 메모리 회수
	u32int heapAddess = (u32int)pProcess->m_lpHeap;
	heapAddess = heapAddess - (heapAddess % PAGE_SIZE);
	for (int i = 0; i < DEFAULT_HEAP_PAGE_COUNT; i++)
	{
		uint32_t virt = heapAddess + (i * PAGE_SIZE);
		VirtualMemoryManager::UnmapPhysicalAddress(pProcess->m_pPageDirectory, virt);
	}

	//페이지 디렉토리 회수
	//페이지 디렉토리는 물리 주소임
	PhysicalMemoryManager::FreeBlock(pProcess->m_pPageDirectory);
	m_processList.Delete(pProcess);
	//프로세스 객체 완전히 제거
#ifdef _ORANGE_DEBUG
	SkyConsole::Print("terminate %s\n", pProcess->m_processName);
#endif // _ORANGE_DEBUG

	delete pProcess;

	return true;
}*/

bool ProcessManager::DestroyProcess(Process* pProcess)
{	
	Process::ThreadList::iterator iter = pProcess->m_threadList.begin();

	for (; iter != pProcess->m_threadList.end(); iter++)
	{
		m_terminatedTaskList.push_back(iter->second);
	}	

	return true;
}


bool ProcessManager::RemoveTerminatedProcess()
{
	/*int threadCount = m_terminatedTaskList.CountItems();

	for (int i = 0; i < threadCount; i++)
	{
		Thread* pThread = (Thread*)m_terminatedTaskList.GetHead()->_data;

		ListNode* pNode = m_terminatedTaskList.Remove(pThread);

		if (pNode)
		{
			DestroyKernelProcess(pThread->m_pParent);
			delete pNode;
		}
		else
		{
			SkyConsole::Print("task delete fail %d\n", pThread->m_threadId);
		}
	}*/

	return true;
}

//커널 프로세스를 종료한다.
bool ProcessManager::DestroyKernelProcess(Process* pProcess)
{
	if (nullptr == pProcess)
		return false;

	//태스크 목록에서 프로세스의 태스크들을 제거한다.
	RemoveFromTaskList(pProcess);

	//스레드 관련 Context 자원을 해제한다.
	//가상 주소를 운영하기 위해 할당했던 페이지 테이블, 스택 등을 회수 등등
	//ReleaseThreadContext(pProcess);

	//페이지 디렉토리 회수
	//페이지 디렉토리는 물리 주소임
	//PhysicalMemoryManager::FreeBlock(pProcess->m_pPageDirectory);
	m_processList.erase(pProcess->m_processId);
	//프로세스 객체 완전히 제거	
#ifdef _SKY_DEBUG
	SkyConsole::Print("terminate %s\n", pProcess->m_processName);
#endif //

	delete pProcess;

	return true;
}

bool ProcessManager::ReleaseThreadContext(Process* pProcess)
{
	/*int threadCount = pProcess->m_threadList.CountItems();

	for (int i = 0; i < threadCount; i++)
	{
		Thread* pThread = pProcess->GetThread(i);
		//스택 페이지 회수
		VirtualMemoryManager::UnmapPhysicalAddress(pProcess->GetPageDirectory(), (uint32_t)pThread->m_initialStack);

		// TLS 등등을 회수		
	}*/

	return true;
}
