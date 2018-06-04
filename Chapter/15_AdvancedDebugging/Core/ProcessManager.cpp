#include "SkyOS.h"
#include "KernelProcessLoader.h"
#include "UserProcessLoader.h"

ProcessManager* ProcessManager::m_processManager = nullptr;
static int kernelStackIndex = 1;
extern int g_stackPhysicalAddressPool;

#define TASK_GENESIS_ID 1000

ProcessManager::ProcessManager()
{
	m_nextThreadId = 1000;

	m_pCurrentTask = nullptr;
	m_pKernelProcessLoader = new KernelProcessLoader();
	m_pUserProcessLoader = new UserProcessLoader();

	m_taskList = new TaskList();
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
	int readCnt = StorageManager::GetInstance()->ReadFile(file, buf, 1, 512);
	if (0 == readCnt)
		return nullptr;

	//유효하지 않은 PE파일이면 파일 핸들을 닫고 종료한다.
	if (!ValidatePEImage(buf))
	{
		SkyConsole::Print("Invalid PE Format!! %s\n", pProcess->m_processName);
		StorageManager::GetInstance()->CloseFile(file);
		return nullptr;
	}

	dosHeader = (IMAGE_DOS_HEADER*)buf;
	ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)buf);

	pProcess->m_imageBase = ntHeaders->OptionalHeader.ImageBase;
	pProcess->m_imageSize = ntHeaders->OptionalHeader.SizeOfImage;

	Thread* pThread = new Thread();
	pThread->m_pParent = pProcess;	
	pThread->m_imageBase = ntHeaders->OptionalHeader.ImageBase;
	pThread->m_imageSize = ntHeaders->OptionalHeader.SizeOfImage;
	pThread->m_dwPriority = 1;
	pThread->m_taskState = TASK_STATE_INIT;
	pThread->m_initialStack = 0;
	pThread->m_stackLimit = PAGE_SIZE;
	pThread->SetThreadId(m_nextThreadId++);

	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)ntHeaders->OptionalHeader.AddressOfEntryPoint + ntHeaders->OptionalHeader.ImageBase;
	pThread->frame.flags = 0x200;


//#ifdef _DEBUG
	SkyConsole::Print("Process ImageBase : 0x%X\n", ntHeaders->OptionalHeader.ImageBase);
//#endif	
	

//파일로부터 읽은 데이터 페이지 수 계산
	int pageRest = 0;

	if ((pThread->m_imageSize % 4096) > 0)
		pageRest = 1;

	pProcess->m_dwPageCount = (pThread->m_imageSize / 4096) + pageRest;

	//파일을 메모리에 할당하는데 필요한 물리 메모리 할당
	unsigned char* physicalMemory = (unsigned char*)PhysicalMemoryManager::AllocBlocks(pProcess->m_dwPageCount);

//물리주소를 가상주소로 매핑한다
//주의 현재 실행중인 프로세스의 가상주소와 생성될 프로세스의 가상주소에 로드된 실행파일의 물리주소를 똑같이 매핑한 후
//복사가 완료되면 현재 실행중인 프로세스에 생성된 PTE를 삭제한다.

	for (DWORD i = 0; i < pProcess->m_dwPageCount; i++)
	{
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(GetCurrentTask()->m_pParent->GetPageDirectory(),
			ntHeaders->OptionalHeader.ImageBase + i * PAGE_SIZE,
			(uint32_t)physicalMemory + i * PAGE_SIZE,
			I86_PTE_PRESENT | I86_PTE_WRITABLE);
	}

	for (DWORD i = 0; i < pProcess->m_dwPageCount; i++)
	{
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->GetPageDirectory(),
			ntHeaders->OptionalHeader.ImageBase + i * PAGE_SIZE,
			(uint32_t)physicalMemory + i * PAGE_SIZE,
			I86_PTE_PRESENT | I86_PTE_WRITABLE);
	}
	
	memory = (unsigned char*)ntHeaders->OptionalHeader.ImageBase;
	memset(memory, 0, pThread->m_imageSize);
	memcpy(memory, buf, 512);

	//파일을 메모리로 로드한다.
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
	
	//스택을 생성하고 주소공간에 매핑한다.
	void* stackAddress = (void*)(g_stackPhysicalAddressPool - PAGE_SIZE * 10 * kernelStackIndex++);

	//스레드에 ESP, EBP 설정
	pThread->m_initialStack = (void*)((uint32_t)stackAddress + PAGE_SIZE * 10);
	pThread->frame.esp = (uint32_t)pThread->m_initialStack;
	pThread->frame.ebp = pThread->frame.esp;

	pThread->frame.eax = 0;
	pThread->frame.ecx = 0;
	pThread->frame.edx = 0;
	pThread->frame.ebx = 0;
	pThread->frame.esi = 0;
	pThread->frame.edi = 0;

//파일 로드에 사용된 페이지 테이블을 회수한다.
	for (DWORD i = 0; i < pProcess->m_dwPageCount; i++)
	{
		VirtualMemoryManager::UnmapPageTable(GetCurrentTask()->m_pParent->GetPageDirectory(), (uint32_t)physicalMemory + i * PAGE_SIZE);
	}
	
	m_taskList->push_back(pThread);
	
	return pThread;
}

Thread* ProcessManager::CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param)
{
	//스레드 객체를 생성하고 변수를 초기화한다.
	Thread* pThread = new Thread();
	pThread->m_pParent = pProcess; // 부모 프로세스
	pThread->SetThreadId(m_nextThreadId++); //스레드 아이디 설정
	pThread->m_dwPriority = 1; //우선순위
	pThread->m_taskState = TASK_STATE_INIT; //태스크 상태
	pThread->m_waitingTime = TASK_RUNNING_TIME; //실행시간
	pThread->m_stackLimit = STACK_SIZE; //스택 크기
	pThread->m_imageBase = 0;
	pThread->m_imageSize = 0;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)lpStartAddress; //EIP
	pThread->frame.flags = 0x200; //플래그
	pThread->m_startParam = param; //파라메터

	//스택을 생성하고 주소공간에 매핑한다.
	void* stackAddress = (void*)(g_stackPhysicalAddressPool - PAGE_SIZE * 10 * kernelStackIndex++);	

#ifdef _DEBUG
	SkyConsole::Print("Stack : %x\n", stackAddress);	
#endif	

	//ESP
	pThread->m_initialStack = (void*)((uint32_t)stackAddress + STACK_SIZE);
	pThread->frame.esp = (uint32_t)pThread->m_initialStack; //ESP
	pThread->frame.ebp = pThread->frame.esp; //EBP

	//태스크 리스트에 스레드를 추가한다.
	m_taskList->push_back(pThread);

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

	SKY_ASSERT(pThread != nullptr, "MainThread is null.");

	bool result = pProcess->AddMainThread(pThread);

	SKY_ASSERT(result == true, "AddMainThread Method Failed.");

	result = AddProcess(pProcess);

	SKY_ASSERT(result == true, "AddProcess Method Failed.");

#ifdef _SKY_DEBUG
	SkyConsole::Print("Process Created. Process Id : %d\n", pProcess->GetProcessId());
#endif

	return pProcess;
}

Process* ProcessManager::CreateProcessFromFile(char* appName, void* param, UINT32 processType)
{
	FILE* file;

	SkyConsole::Print("Open File : %s\n", appName);

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

	if (pThread == nullptr)
		return nullptr;

	bool result = pProcess->AddMainThread(pThread);

	SKY_ASSERT(result == true, "AddMainThread Method Failed.");

	kEnterCriticalSection();
	result = AddProcess(pProcess);
	kLeaveCriticalSection();

	SKY_ASSERT(result == true, "AddProcess Method Failed.");

#ifdef _SKY_DEBUG
	SkyConsole::Print("Process Created. Process Id : %d\n", pProcess->GetProcessId());
#endif

	StorageManager::GetInstance()->CloseFile(file);

	return pProcess;
}

Process* ProcessManager::FindProcess(int processId)
{
	ProcessList::iterator iter = m_processList.find(processId);

	if (iter == m_processList.end())
		return nullptr;

	return (*iter).second;
}
Thread* ProcessManager::FindTask(DWORD taskId)
{
	auto iter = m_taskList->begin();
	for (; iter != m_taskList->end(); iter++)
	{
		Thread* pThread = *iter;
		if (pThread->GetThreadId() == taskId)
			return pThread;
	}

	return nullptr;
}


bool ProcessManager::AddProcess(Process* pProcess)
{
	if (pProcess == nullptr)
		return false;

	int entryPoint = 0;
	unsigned int procStack = 0;

	if (pProcess->GetProcessId() == PROC_INVALID_ID)
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

	m_processList[pProcess->GetProcessId()] = pProcess;
	
	return true;
}

bool ProcessManager::RemoveProcess(int processId)
{
	kEnterCriticalSection();

	map<int, Process*>::iterator iter = m_processList.find(processId);

	Process* pProcess = (*iter).second;

	if (pProcess == nullptr)
	{
		kLeaveCriticalSection();
		return false;
	}

	m_processList.erase(iter);	

	map<int, Thread*>::iterator threadIter = pProcess->m_threadList.begin();

	for (; threadIter != pProcess->m_threadList.end(); threadIter++)
	{
		Thread* pThread = (*threadIter).second;				
		m_taskList->remove(pThread);
		delete pThread;
	}	

	pProcess->m_threadList.clear();

	if (pProcess->m_lpHeap)
	{
		SkyConsole::Print("default heap free : 0x%x\n", pProcess->m_lpHeap);
		delete pProcess->m_lpHeap;
		pProcess->m_lpHeap = nullptr;
	}

	VirtualMemoryManager::FreePageDirectory(pProcess->GetPageDirectory());

	delete pProcess;
	
	kLeaveCriticalSection();

	return true;
}

bool ProcessManager::RemoveTerminatedProcess()
{
	//kEnterCriticalSection();

	auto iter = m_terminatedProcessList.begin();
	for (; iter != m_terminatedProcessList.end(); iter++)
	{
		RemoveProcess((*iter).second->GetProcessId());
	}
	
	m_terminatedProcessList.clear();
	
	/*for (; iter != m_terminatedProcessList.end(); iter++)
	{
		Process* pProcess = (*iter).second;

		if (pProcess == nullptr)
			continue;

		m_processList.erase(iter);
		map<int, Thread*>::iterator threadIter = pProcess->m_threadList.begin();

		for (; threadIter != pProcess->m_threadList.end(); threadIter++)
		{
			Thread* pThread = (*threadIter).second;
			m_taskList.remove(pThread);
			delete pThread;
		}

		pProcess->m_threadList.clear();
		VirtualMemoryManager::FreePageDirectory(pProcess->GetPageDirectory());
		delete pProcess;
	}

	m_terminatedProcessList.clear();*/

	//kLeaveCriticalSection();

	return true;
}

bool ProcessManager::ReserveRemoveProcess(Process* pProcess)
{
	m_terminatedProcessList[pProcess->GetProcessId()] = pProcess;
	return true;
}