#include "main.h"
#include "stdint.h"
#include "header.h"
#include "string.h"
#include "sprintf.h"
#include "Console.h"
#include "HAL.h"
#include "RTC.H"
#include "IDT.h"
#include "GDT.h"
#include "PIT.h"
#include "PIC.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"
#include "kheap.h"
#include "ZetPlane.h"
#include "sysapi.h"
#include "tss.h"
#include "ProcessManager.h"
#include "ConsoleManager.h"
#include "List.h"
#include "KernelProcedure.h"
#include "Console.h"
#include "InitKernel.h"
#include "FAT.h"

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
			call    main; kernel entry
			halt :
		jmp halt; halt processor
	}
}

extern bool systemOn;
bool InitMemoryManager(multiboot_info* bootinfo, uint32_t kernelSize);
void StartConsoleSystem();

void main(unsigned long magic, unsigned long addr)
{
	InitializeConstructors();

	SkyConsole::Initialize();
	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.	
	SkyConsole::Print("*** Sky OS Console System Init\n");

	EnterCriticalSection();

	multiboot_info* pBootInfo = (multiboot_info*)addr;	
	
	i86_gdt_initialize();			
	i86_idt_initialize(0x8);		
	i86_pic_initialize(0x20, 0x28);
	i86_pit_initialize();
											
	SetInterruptVector();			
	InitMemoryManager(pBootInfo, 0);

	InitKeyboard();	

	//InitFloppyDrive();
	
	InitializeSysCall();	
			
	i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	LeaveCriticalSection();	

	/*InitHardDrive();

	InitFATFileSystem();

	UINT16 handle = FATFileOpen("C:\MENU.LST", 0);
	
	if (handle != 0)
	{
		SkyConsole::Print("FileHandle : %x\n", handle);
		BYTE* buffer = new BYTE[512];

		FATReadFile(1, 512, buffer);

		SkyConsole::Print("%s\n", buffer);
	}*/
	
	
	//! initialize TSS
	//install_tss(5, 0x10, 0);

	DumpSystemInfo();
	SkyConsole::Print("Boot Loader Name : %s\n", (char*)pBootInfo->boot_loader_name);

	drive_info* pDriveInfo = pBootInfo->drives_addr;

	//for (int i = 0; i < pBootInfo->drives_length; i++)
	/*{
		SkyConsole::Print("Drive Structures Total Length : %d\n", pBootInfo->drives_length);
		SkyConsole::Print("Drive Size : %d\n", pDriveInfo->size);
		SkyConsole::Print("Drive Number : %d\n", pDriveInfo->drive_number);
		SkyConsole::Print("Drive Mode : %d\n", pDriveInfo->drive_mode);

		SkyConsole::Print("Drive Cylinder : %d\n", pDriveInfo->drive_cylinders);
		SkyConsole::Print("Drive Head : %d\n", pDriveInfo->drive_heads);		
		SkyConsole::Print("Drive Sector : %d\n", pDriveInfo->drive_sectors);		

	}*/

	InitGraphics(pBootInfo);
	

	/*SkyConsole::Print("Press Any Key\n");
	SkyConsole::GetChar();*/
	StartConsoleSystem();
}

uint32_t GetTotalMemory(multiboot_info* bootinfo)
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

		//SkyConsole::Print("Memory Address : %x\n", entry->addr);
		//DebugSkyConsole::Print("\nMemory Length : %x", entry->len);
		//DebugSkyConsole::Print("\nMemory Type : %x", entry->type);
		//DebugSkyConsole::Print("\nEntry Size : %d", entry->size);

		mmapAddr += sizeof(multiboot_memory_map_t);
		memorySize += entry->len;
	}

	return memorySize;
}

// where the kernel is to be loaded to in protected mode
// define IMAGE_PMODE_BASE 0xC0000000; 0x100000
//  where the kernel is to be loaded to in real mode
//  define IMAGE_RMODE_BASE 0x3000

bool InitMemoryManager(multiboot_info* bootinfo, uint32_t kernelSize)
{
	uint32_t totalMemorySize = GetTotalMemory(bootinfo);

	SkyConsole::Print("KernelSize : %d Bytes\n", kernelSize);
	SkyConsole::Print("TotalMemorySize : 0x%x Bytes\n", totalMemorySize);

	if (PhysicalMemoryManager::IsPaging() == true)
	{
		PhysicalMemoryManager::EnablePaging(false);
		SkyConsole::Print("Paging\n");
	}	
	
	PhysicalMemoryManager::Initialize(totalMemorySize, KERNEL_STACK);
	PhysicalMemoryManager::Dump();

	uint32_t memoryMapSize = PhysicalMemoryManager::GetMemoryMapSize();
	uint32_t alignedMemoryMapSize = (memoryMapSize / 4096) * 4096;

	if (memoryMapSize % 4096 > 0)
		alignedMemoryMapSize += 4096;

	PhysicalMemoryManager::SetAvailableMemory(KERNEL_STACK + alignedMemoryMapSize, totalMemorySize - (KERNEL_STACK + alignedMemoryMapSize));

	PhysicalMemoryManager::Dump();
	
	//! initialize our vmm
	VirtualMemoryManager::Initialize();	

	return true;
}


void StartConsoleSystem()
{		
	Process* pProcess = ProcessManager::GetInstance()->CreateProcess(SystemEntry, true);

	if (pProcess)
		SkyConsole::Print("Create Success System Process\n");

	//Thread* newThread = ProcessManager::CreateThread(pProcess, SampleLoop, pProcess);
	//Thread* newThread2 = ProcessManager::CreateThread(pProcess, TaskProcessor, pProcess);
	
	Thread* pThread = pProcess->GetThread(0);
	pThread->m_taskState = TASK_STATE_RUNNING;	

	int entryPoint = (int)pThread->frame.eip;
	unsigned int procStack = pThread->frame.esp;

	EnterCriticalSection();

	systemOn = true;
	
	__asm
	{
		mov     ax, 0x10;
		mov     ds, ax
			mov     es, ax
			mov     fs, ax
			mov     gs, ax
			;
		; create stack frame
			;
		; push   0x10;
		; push procStack; stack
			mov esp, procStack
			push   0x10;
			push    0x200; EFLAGS
			push    0x08; CS
			push entryPoint; EIP
			iretd
	}
}