#include "kmain.h"

extern bool systemOn;

_declspec(naked) void multiboot_entry(void)
{
	__asm {
		align 4

		multiboot_header:
		dd(MULTIBOOT_HEADER_MAGIC); magic number
			dd(MULTIBOOT_HEADER_FLAGS); flags
			dd(CHECKSUM); checksum
			dd(HEADER_ADRESS); header address
			dd(KERNEL_LOAD_ADDRESS); load address
			dd(00); load end address : not used
			dd(00); bss end addr : not used
			dd(HEADER_ADRESS + 0x20); entry_addr: equ kernel entry
			; 0x20 is the size of multiboot header

			kernel_entry :
		mov     esp, KERNEL_STACK; Setup the stack

			push    0; Reset EFLAGS
			popf

			push    ebx; Push multiboot info address
			push    eax; and magic number
			; which are loaded in registers
			; eax and ebx before jump to
			; entry adress
			; [HEADER_ADRESS + 0x20]
			call    kmain; kernel entry
			halt :
		jmp halt; halt processor
	}
}

void kmain(unsigned long magic, unsigned long addr)
{
	InitializeConstructors();

	SkyConsole::Initialize();
	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.	
	SkyConsole::Print("*** Sky OS Console System Init\n");

	EnterCriticalSection();	
	
	i86_gdt_initialize();			
	i86_idt_initialize(0x8);		
	i86_pic_initialize(0x20, 0x28);
	i86_pit_initialize();
											
	SetInterruptVector();	
	InitializeSysCall();
	//! initialize TSS
	install_tss(5, 0x10, 0);

	InitKeyboard();

	multiboot_info* pBootInfo = (multiboot_info*)addr;
	InitMemoryManager(pBootInfo, 0);	

	InitFloppyDrive();
			
	i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	__asm sti
	if (true == InitHardDrive())
	{
		InitFATFileSystem();
#ifdef _SKY_DEBUG
		TestHardDrive();
#endif
	}


	//Initialize FILESYSTEM
	//VFSInitialize();

	__asm cli

	LeaveCriticalSection();
	

	DumpSystemInfo(pBootInfo);
	SkyConsole::Print("Boot Loader Name : %s\n", (char*)pBootInfo->boot_loader_name);

	bool consoleMode = true;

	if (consoleMode == true)
	{
		StartConsoleSystem();
	}
	else
	{
		InitGraphics(pBootInfo);
	}
}

uint32_t GetFreeSpaceMemory(multiboot_info* bootinfo)
{
	uint32_t memorySize = 0;
	uint32_t mmapEntryNum = bootinfo->mmap_length / sizeof(multiboot_memory_map_t);

	SkyConsole::Print("GRUB Impormation\n");
	SkyConsole::Print("Boot Loader Name : %s\n", (char*)bootinfo->boot_loader_name);
	SkyConsole::Print("Memory Map Entry Num : %d\n", mmapEntryNum);

	uint32_t mmapAddr = bootinfo->mmap_addr;

	for (uint32_t i = 0; i < mmapEntryNum; i++)
	{
		multiboot_memory_map_t* entry = (multiboot_memory_map_t*)mmapAddr;

#ifdef _SKY_DEBUG
		SkyConsole::Print("Memory Address : %x\n", entry->addr);
		SkyConsole::Print("\nMemory Length : %x", entry->len);
		SkyConsole::Print("\nMemory Type : %x", entry->type);
		SkyConsole::Print("\nEntry Size : %d", entry->size);
#endif

		mmapAddr += sizeof(multiboot_memory_map_t);

		if (entry->addr + entry->len < FREE_MEMORY_SPACE_ADDRESS)
			continue;

		memorySize += entry->len;
		if (entry->addr < FREE_MEMORY_SPACE_ADDRESS)
			memorySize -= (FREE_MEMORY_SPACE_ADDRESS - entry->addr);
	}

	memorySize -= (memorySize % 4096);

	return memorySize;
}

// where the kernel is to be loaded to in protected mode
// define IMAGE_PMODE_BASE 0xC0000000; 0x100000
//  where the kernel is to be loaded to in real mode
//  define IMAGE_RMODE_BASE 0x3000

bool InitMemoryManager(multiboot_info* bootinfo, uint32_t kernelSize)
{
	PhysicalMemoryManager::EnablePaging(false);

	uint32_t freeSpaceMemorySize = GetFreeSpaceMemory(bootinfo);

	//SkyConsole::Print("KernelSize : %d Bytes\n", kernelSize);
	SkyConsole::Print("FreeSpace MemorySize From 0x%x: 0x%x Bytes\n", FREE_MEMORY_SPACE_ADDRESS, freeSpaceMemorySize);
	
	
//물리 메모리 매니저 초기화
	PhysicalMemoryManager::Initialize(freeSpaceMemorySize, FREE_MEMORY_SPACE_ADDRESS);
	//PhysicalMemoryManager::Dump();

	uint32_t memoryMapSize = PhysicalMemoryManager::GetMemoryMapSize();
	uint32_t alignedMemoryMapSize = (memoryMapSize / 4096) * 4096;

	if (memoryMapSize % 4096 > 0)
		alignedMemoryMapSize += 4096;

	PhysicalMemoryManager::SetAvailableMemory(FREE_MEMORY_SPACE_ADDRESS + alignedMemoryMapSize, freeSpaceMemorySize - (FREE_MEMORY_SPACE_ADDRESS + alignedMemoryMapSize));
	//PhysicalMemoryManager::Dump();
	
//가상 메모리 매니저 초기화	
	VirtualMemoryManager::Initialize();	



	//커널 힙을 생성한다
	VirtualMemoryManager::CreateKernelHeap();

	return true;
}


void StartConsoleSystem()
{		
	EnterCriticalSection();

	Process* pProcess = ProcessManager::GetInstance()->CreateConsoleProcess(SystemEntry);

	if (pProcess == nullptr)
		HaltSystem("Console Creation Fail!!");
	
	SkyConsole::Print("Init Console....\n");

	Thread* pThread = pProcess->GetThread(0);
	pThread->m_taskState = TASK_STATE_RUNNING;	

	int entryPoint = (int)pThread->frame.eip;
	unsigned int procStack = pThread->frame.esp;

	JumpToNewKernelEntry(entryPoint, procStack);

//쓸모없는 코드지만 쌍을 맞추기 위해
	LeaveCriticalSection();
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
			
		; create stack frame			
		; push   0x10;
		; push procStack; stack
		mov esp, procStack
		push	0x10;
		push    0x200; EFLAGS
		push    0x08; CS
		push entryPoint; EIP
		sti
		iretd
	}
}