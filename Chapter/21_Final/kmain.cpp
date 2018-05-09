#include "kmain.h"
#include "SkyTest.h"

#include "PCI.h"
#include "bepci.h"
#include "nic.h"

_declspec(naked) void multiboot_entry(void)
{
	__asm {
		align 4

		multiboot_header:
		//멀티부트 헤더 사이즈 : 0X30
		dd(MULTIBOOT_HEADER_MAGIC); magic number

#if SKY_CONSOLE_MODE == 0
		dd(MULTIBOOT_HEADER_FLAGS_GUI); flags
		dd(CHECKSUM_GUI); checksum
#else
		dd(MULTIBOOT_HEADER_FLAGS); flags
		dd(CHECKSUM); checksum
#endif		
		dd(HEADER_ADRESS); //헤더 주소 KERNEL_LOAD_ADDRESS+ALIGN(0x100400)
		dd(KERNEL_LOAD_ADDRESS); //커널이 로드된 가상주소 공간
		dd(00); //사용되지 않음
		dd(00); //사용되지 않음
		dd(HEADER_ADRESS + 0x30); //커널 시작 주소 : 멀티부트 헤더 주소 + 0x30, kernel_entry

		dd(SKY_CONSOLE_MODE);
		dd(SKY_WIDTH);
		dd(SKY_HEIGHT);
		dd(SKY_BPP)

	kernel_entry:
		MOV     ESP, 0x40000; //스택 설정

		PUSH    0; //플래그 레지스터 초기화
		POPF

		//GRUB에 의해 담겨 있는 정보값을 스택에 푸쉬한다.
		PUSH    EBX; //멀티부트 구조체 포인터
		PUSH    EAX; //매직 넘버

		//위의 두 파라메터와 함께 kmain 함수를 호출한다.
		CALL    kmain; //C++ 메인 함수 호출

		//루프를 돈다. kmain이 리턴되지 않으면 아래 코드는 수행되지 않는다.
	halt:
		jmp halt;
	}
}

bool systemOn = false;
bool g_heapInit = false;

extern PageDirectory* pageDirectoryPool[10];
int g_stackPhysicalAddressPool = 0;

void HardwareInitialize();
bool InitMemoryManager(multiboot_info* bootinfo);
void ConstructFileSystem(multiboot_info* info);
void StartConsoleSystem();
void StartGUISystem();

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack);

void kmain(unsigned long magic, unsigned long addr)
{
	InitializeConstructors();		

	multiboot_info* pBootInfo = (multiboot_info*)addr;

	SkyConsole::Initialize();
	
	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.	
	SkyConsole::Print("*** Sky OS Console System Init ***\n");

	SkyConsole::Print("GRUB Information\n");
	SkyConsole::Print("Boot Loader Name : %s\n", (char*)pBootInfo->boot_loader_name);
	
	kEnterCriticalSection();

	HardwareInitialize();
	SkyConsole::Print("Hardware Init Complete\n");

	SetInterruptVector();
	InitializeSysCall();
	//현재 구조에서는 TSS가 필요없음
	//InstallTSS(5, 0x10, 0);

	SkyConsole::Print("Interrput Handler Init Complete\n");

	if (false == InitFPU())
	{
		SkyConsole::Print("[Warning] Floating Pointer Unit Detection Fail\n");
	}
	else
	{
		EnableFPU();
		SkyConsole::Print("FPU Init..\n");
	}

	//물리/가상 메모리 매니저를 초기화한다.
	//설정 시스템 메모리는 128MB
	InitMemoryManager(pBootInfo);

	SkyConsole::Print("Memory Manager Init Complete\n");

	int heapFrameCount = 256 * 10 * 5; //프레임수 12800개, 52MB
	unsigned int requiredHeapSize = heapFrameCount * PAGE_SIZE;

	//요구되는 힙의 크기가 자유공간보다 크다면 그 크기를 자유공간 크기로 맞춘다음 반으로 줄인다.
	//if (requiredHeapSize > g_freeMemorySize)
	//{
		//requiredHeapSize = g_freeMemorySize;
		//heapFrameCount = requiredHeapSize / PAGE_SIZE / 2;
	//}

	HeapManager::InitKernelHeap(heapFrameCount);
	SkyConsole::Print("Heap %dMB Allocated\n", requiredHeapSize / 1048576);	

	g_heapInit = true;

#if SKY_CONSOLE_MODE == 0
	SkyGUISystem::GetInstance()->Initialize(pBootInfo);
#else
	InitKeyboard();
	SkyConsole::Print("Keyboard Init..\n");
#endif

	UINT16 pciDevices = InitPCIDevices();
	SkyConsole::Print("%d device(s) found\n", pciDevices);	
	
	ConstructFileSystem(pBootInfo);

	kLeaveCriticalSection();

	GlobalSate state;
	state._HeapLoadAddress = KERNEL_VIRTUAL_HEAP_ADDRESS;
	state._heapSize = HeapManager::GetHeapSize();
	state._kernelLoadAddress = KERNEL_LOAD_ADDRESS;
	state._kernelSize = PhysicalMemoryManager::GetKernelEnd() - KERNEL_LOAD_ADDRESS;

	g_stackPhysicalAddressPool = PAGE_ALIGN_UP(PhysicalMemoryManager::GetKernelEnd() + PhysicalMemoryManager::GetMemoryMapSize() + 1048576);
	state._stackPhysicalPoolAddress = g_stackPhysicalAddressPool;
	state._pciDevices = pciDevices;
	state._pageDirectoryPoolAddress = (DWORD)&(pageDirectoryPool[0]);

	SystemProfiler::GetInstance()->SetGlobalState(state);
	
	scan_pci_devices();
	//init_nic();	
	TestLua();

	SYSTEMTIME time;
	GetLocalTime(&time);
	char buffer[256];
	sprintf(buffer, "Current Time : %d/%d/%d %d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	SkyConsole::Print("%s", buffer);

	ProcessManager::GetInstance();
	Scheduler::GetInstance();
	
#if SKY_CONSOLE_MODE == 0	
	StartGUISystem();
#else
	StartConsoleSystem();
#endif
	
	for (;;);	
}

void HardwareInitialize()
{
	GDTInitialize();
	IDTInitialize(0x8);
	PICInitialize(0x20, 0x28);
	PITInitialize();
}


bool InitMemoryManager(multiboot_info* bootinfo)
{
	PhysicalMemoryManager::EnablePaging(false);

	//물리 메모리 매니저 초기화
	PhysicalMemoryManager::Initialize(bootinfo);
	//PhysicalMemoryManager::Dump();

	//가상 메모리 매니저 초기화	
	VirtualMemoryManager::Initialize();
	//PhysicalMemoryManager::Dump();

	return true;
}

void ConstructFileSystem(multiboot_info* info)
{	
//IDE 하드 디스크
	FileSysAdaptor* pHDDAdaptor = new HDDAdaptor("HardDisk", 'C');
	
	pHDDAdaptor->Initialize();

	if (pHDDAdaptor->GetCount() > 0)
	{
		StorageManager::GetInstance()->RegisterFileSystem(pHDDAdaptor, 'C');
		StorageManager::GetInstance()->SetCurrentFileSystemByID('C');
		
		//TestHardDisk();			
		//TestLua();
	}
	else
	{
		delete pHDDAdaptor;		
	}
			
//램 디스크
	FileSysAdaptor* pRamDiskAdaptor = new RamDiskAdaptor("RamDisk", 'K');
	if (pRamDiskAdaptor->Initialize() == true)
	{
		StorageManager::GetInstance()->RegisterFileSystem(pRamDiskAdaptor, 'K');
		StorageManager::GetInstance()->SetCurrentFileSystemByID('K');		

		((RamDiskAdaptor*)pRamDiskAdaptor)->InstallPackage();
	}
	else
	{
		delete pRamDiskAdaptor;
	}

//플로피 디스크
	/*FileSysAdaptor* pFloppyDiskAdaptor = new FloppyDiskAdaptor("FloppyDisk", 'A');
	if (pFloppyDiskAdaptor->Initialize() == true)
	{
		StorageManager::GetInstance()->RegisterFileSystem(pFloppyDiskAdaptor, 'A');
		StorageManager::GetInstance()->SetCurrentFileSystemByID('A');
		
	}
	else
	{
		delete pFloppyDiskAdaptor;
	}*/	

	StorageManager::GetInstance()->SetCurrentFileSystemByID('C');
	SkyConsole::Print("K drive Selected\n");

	drive_info* driveInfo = info->drives_addr;

	for (uint32_t i = 0; i < info->drives_length; i++)
	{
		int driveNum = driveInfo[i].drive_number;

		if (driveNum != 0)
			SkyConsole::Print("%d drive Detected\n", driveNum);
	}
}

void StartConsoleSystem()
{
	kEnterCriticalSection();

	Process* pProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("ConsoleSystem", SystemConsoleProc, NULL, PROCESS_KERNEL);

	if (pProcess == nullptr)
		HaltSystem("Console Creation Fail!!");

	ProcessManager::GetInstance()->CreateProcessFromMemory("WatchDog", WatchDogProc, NULL, PROCESS_KERNEL);
	ProcessManager::GetInstance()->CreateProcessFromMemory("ProcessRemover", ProcessRemoverProc, NULL, PROCESS_KERNEL);
	
	SkyConsole::Print("Init Console....\n");

	Thread* pThread = pProcess->GetMainThread();

	if (pThread == nullptr)
		HaltSystem("Console Creation Fail!!");

	pThread->m_taskState = TASK_STATE_RUNNING;

	int entryPoint = (int)pThread->frame.eip;
	unsigned int procStack = pThread->frame.esp;

	kLeaveCriticalSection();

	SkyConsole::Print("ConsoleSystem : entryPoint : (0x%x)\n", entryPoint);
	SkyConsole::Print("ConsoleSystem : procStack : (0x%x)\n", procStack);

	ProcessManager::GetInstance()->SetCurrentTask(pThread);
	
	JumpToNewKernelEntry(entryPoint, procStack);
}

void StartGUISystem()
{
	kEnterCriticalSection();

	Process* pProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("GUISystem", SystemGUIProc, NULL, PROCESS_KERNEL);

	if (pProcess == nullptr)
		HaltSystem("GUI System Creation Fail!!");
	
	ProcessManager::GetInstance()->CreateProcessFromMemory("ProcessRemover", ProcessRemoverProc, NULL, PROCESS_KERNEL);

	SkyConsole::Print("Init GUI System....\n");

	Thread* pThread = pProcess->GetMainThread();

	pThread->m_taskState = TASK_STATE_RUNNING;

	int entryPoint = (int)pThread->frame.eip;
	unsigned int procStack = pThread->frame.esp;

	kLeaveCriticalSection();

	SkyConsole::Print("GUI System : entryPoint : (0x%x)\n", entryPoint);
	SkyConsole::Print("GUI System : procStack : (0x%x)\n", procStack);

	ProcessManager::GetInstance()->SetCurrentTask(pThread);

	JumpToNewKernelEntry(entryPoint, procStack);
}

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack)
{
	__asm
	{
		MOV     AX, 0x10;
		MOV     DS, AX
		MOV     ES, AX
		MOV     FS, AX
		MOV     GS, AX
			
		MOV     ESP, procStack
		PUSH	0x10;
		PUSH    0x200; EFLAGS
		PUSH    0x08; CS
		PUSH    entryPoint; EIP

		IRETD
	}
}