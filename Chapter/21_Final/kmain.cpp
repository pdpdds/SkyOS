#include "kmain.h"
#include "SkyTest.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"
#include "HeapManager.h"
#include "HDDAdaptor.h"
#include "RamDiskAdaptor.h"
#include "FloppyDiskAdaptor.h"
#include "StorageManager.h"
#include "fileio.h"
#include "SysAPI.h"
#include "FPU.h"
#include "TSS.h"
#include "ProcessManager.h"
#include "KernelProcedure.h"
#include "Scheduler.h"
#include "SystemProfiler.h"
#include "PCI.h"
#include "SysInfo.h"
#include "SkyGUISystem.h"
#include "JPEG.h"
#include "VESA.h"
#include "nic.h"
#include "bepci.h"

//#define SKY_GUI 1
//#define SKY_HARIBOTE
//#define SKY_SVGA


_declspec(naked) void multiboot_entry(void)
{
	__asm {
		align 4

		multiboot_header:
		//멀티부트 헤더 사이즈 : 0X30
		dd(MULTIBOOT_HEADER_MAGIC); magic number

#ifdef SKY_GUI
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
//하리보테 1024 768 0
//SVGA 1024 768 24
		//비디오
#ifdef SKY_GUI

		dd(0);
		dd(1024);
		dd(768);
		dd(32)
#else
		dd(1); //텍스트 모드
		dd(0);
		dd(0);
		dd(0);
#endif

	kernel_entry:
		mov     esp, KERNEL_STACK; //스택 설정

		push    0; //플래그 레지스터 초기화
		popf

		//GRUB에 의해 담겨 있는 정보값을 스택에 푸쉬한다.
		push    ebx; //멀티부트 구조체 포인터
		push    eax; //매직 넘버

		//위의 두 파라메터와 함께 kmain 함수를 호출한다.
		call    kmain; //C++ 메인 함수 호출

		//루프를 돈다. kmain이 리턴되지 않으면 아래 코드는 수행되지 않는다.
	halt:
		jmp halt;
	}
}

bool systemOn = false;

uint32_t g_freeMemoryStartAddress = 0x00400000; //자유공간 시작주소 : 4MB
uint32_t g_freeMemorySize = 0;
extern PageDirectory* pageDirectoryPool[10];

void HardwareInitialize();
bool InitMemoryManager(multiboot_info* bootinfo);
void ConstructFileSystem(multiboot_info* info);
void StartConsoleSystem();
void StartGUISystem();

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack);

BYTE PrintPCIDeviceList(struct PCIConfigurationSpace * ptrPCIDet);

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
	InstallTSS(5, 0x10, 0);

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

	//물/가상 메모리 매니저를 초기화한다.
	//설정 시스템 메모리는 128MB
	InitMemoryManager(pBootInfo);

	SkyConsole::Print("Memory Manager Init Complete\n");

	int heapFrameCount = 256 * 10 * 5; //프레임수 12800개, 52MB
	unsigned int requiredHeapSize = heapFrameCount * PAGE_SIZE;

	//요구되는 힙의 크기가 자유공간보다 크다면 그 크기를 자유공간 크기로 맞춘다음 반으로 줄인다.
	if (requiredHeapSize > g_freeMemorySize)
	{
		requiredHeapSize = g_freeMemorySize;
		heapFrameCount = requiredHeapSize / PAGE_SIZE / 2;
	}

	HeapManager::InitKernelHeap(heapFrameCount);

	SkyConsole::Print("Heap %dMB Allocated\n", requiredHeapSize / 1048576);	

	UINT16 pciDevices = InitPCIDevices();
	SkyConsole::Print("%d device(s) found\n", pciDevices);
	
	InitKeyboard();
	SkyConsole::Print("Keyboard Init..\n");
	
	ConstructFileSystem(pBootInfo);

	kLeaveCriticalSection();

	ProcessManager::GetInstance();
	Scheduler::GetInstance();

	GlobalSate state;
	state._HeapLoadAddress = KERNEL_VIRTUAL_HEAP_ADDRESS;
	state._heapSize = HeapManager::GetHeapSize();
	state._kernelLoadAddress = KERNEL_VIRTUAL_BASE_ADDRESS;
	state._kernelSize = KERNEL_END_ADDRESS;
	state._stackPhysicalPoolAddress = KERNEL_STACK;
	state._pciDevices = pciDevices;
	state._pageDirectoryPoolAddress = (DWORD)&(pageDirectoryPool[0]);

	SystemProfiler::GetInstance()->SetGlobalState(state);

	
	scan_pci_devices();
	init_nic();
	for (;;);
	
#ifdef SKY_GUI
	SkyGUISystem::GetInstance()->Initialize(pBootInfo);
	init_lfb(pBootInfo->vbe_mode_info);
	lfb_clear();

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
	InitializePIT();
}

uint32_t GetFreeSpaceMemory(multiboot_info* bootinfo)
{
	uint32_t memorySize = 0;

	memorySize = 128 * MEGA_BYTES;
	return memorySize;

	uint32_t mmapEntryNum = bootinfo->mmap_length / sizeof(multiboot_memory_map_t);

	uint32_t mmapAddr = bootinfo->mmap_addr;

#ifdef _SKY_DEBUG
	SkyConsole::Print("Memory Map Entry Num : %d\n", mmapEntryNum);
#endif

	for (uint32_t i = 0; i < mmapEntryNum; i++)
	{
		multiboot_memory_map_t* entry = (multiboot_memory_map_t*)mmapAddr;

#ifdef _SKY_DEBUG
		SkyConsole::Print("Memory Address : %x\n", entry->addr);
		SkyConsole::Print("Memory Length : %x\n", entry->len);
		SkyConsole::Print("Memory Type : %x\n", entry->type);
		SkyConsole::Print("Entry Size : %d\n", entry->size);
#endif

		mmapAddr += sizeof(multiboot_memory_map_t);

		if (entry->addr == 0)
			continue;

		if (entry->addr + entry->len < g_freeMemoryStartAddress)
			continue;

		if (memorySize > entry->len)
			continue;

		memorySize = entry->len;

		if (entry->addr < g_freeMemoryStartAddress)
			memorySize -= (g_freeMemoryStartAddress - entry->addr);
		//else
			//g_freeMemoryStartAddress = entry->addr;
	}

	memorySize -= (memorySize % 4096);

	return memorySize;
}

bool InitMemoryManager(multiboot_info* bootinfo)
{
	PhysicalMemoryManager::EnablePaging(false);

	g_freeMemorySize = GetFreeSpaceMemory(bootinfo);

	//물리 메모리 매니저 초기화
	PhysicalMemoryManager::Initialize(g_freeMemorySize, g_freeMemoryStartAddress);
	//PhysicalMemoryManager::Dump();

	//가상 메모리 매니저 초기화	
	VirtualMemoryManager::Initialize();
	//PhysicalMemoryManager::Dump();

	SkyConsole::Print("Free Memory Start Address(0x%x)\n", g_freeMemoryStartAddress);
	SkyConsole::Print("Free Memory Size(%dMB)\n", g_freeMemorySize / 1048576);

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

	StorageManager::GetInstance()->SetCurrentFileSystemByID('K');
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
		mov     ax, 0x10;
		mov     ds, ax
			mov     es, ax
			mov     fs, ax
			mov     gs, ax

			//; create stack frame
			//; push   0x10;
			//; push procStack; stack
			mov     esp, procStack
			push	0x10;
		push    0x200; EFLAGS
			push    0x08; CS
			push    entryPoint; EIP
			iretd
	}
}