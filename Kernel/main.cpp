#include "main.h"
#include "header.h"
#include "DebugDisplay.h"
#include "string.h"
#include "sprintf.h"
#include "Console.h"
#include "HAL.h"
#include "RTC.H"
#include "IDT.h"
#include "GDT.h"
#include "PIT.h"
#include "PIC.h"
#include "exception.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"
#include "kheap.h"
#include "../Mint64/HardDisk.h"
#include "flpydsk.h"
#include "fat12.h"
#include "ZetPlane.h"
#include "sysapi.h"
#include "tss.h"
#include "ProcessManager.h"
#include "ConsoleManager.h"
#include "List.h"
#include "kybrd.h"
#include "Keyboard.h"
#include "KernelProcedure.h"
#include "Console.h"

extern void __cdecl  InitializeConstructors();

void SetInterruptVector();
bool InitializeMemorySystem(multiboot_info* bootinfo, uint32_t kernelSize);
void InitializeFloppyDrive();
void CreateCentralSystem();
void CreateTestKernelProcess();

/* Definitions for BGA. Reference Graphics 1. */
#define VBE_DISPI_IOPORT_INDEX          0x01CE
#define VBE_DISPI_IOPORT_DATA           0x01CF
#define VBE_DISPI_INDEX_XRES            0x1
#define VBE_DISPI_INDEX_YRES            0x2
#define VBE_DISPI_INDEX_BPP             0x3
#define VBE_DISPI_INDEX_ENABLE          0x4
#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_LFB_ENABLED           0x40

/* writes to BGA port. */
void VbeBochsWrite(uint16_t index, uint16_t value) {
	OutPortWord(VBE_DISPI_IOPORT_INDEX, index);
	OutPortWord(VBE_DISPI_IOPORT_DATA, value);
}

/* sets video mode. */
void VbeBochsSetMode(uint16_t xres, uint16_t yres, uint16_t bpp) {
	VbeBochsWrite(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
	VbeBochsWrite(VBE_DISPI_INDEX_XRES, xres);
	VbeBochsWrite(VBE_DISPI_INDEX_YRES, yres);
	VbeBochsWrite(VBE_DISPI_INDEX_BPP, bpp);
	VbeBochsWrite(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

/* video mode info. */
#define WIDTH           800
#define HEIGHT          600
#define BPP             32
#define BYTES_PER_PIXEL 4

/* BGA LFB is at LFB_PHYSICAL. */
#define LFB_PHYSICAL 0xE0000000
#define LFB_VIRTUAL  0x300000
#define PAGE_SIZE    0x1000

/* map LFB into current process address space. */
void* VbeBochsMapLFB() {
	int pfcount = WIDTH*HEIGHT*BYTES_PER_PIXEL / PAGE_SIZE;
	for (int c = 0; c <= pfcount; c++)
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(VirtualMemoryManager::GetCurPageDirectory(), LFB_VIRTUAL + c * PAGE_SIZE, LFB_PHYSICAL + c * PAGE_SIZE, 7);
	return (void*)LFB_VIRTUAL;
}

/* clear screen to white. */
void fillScreen32() {
	uint32_t* lfb = (uint32_t*)LFB_VIRTUAL;
	for (uint32_t c = 0; c<WIDTH*HEIGHT; c++)
		lfb[c] = 0x00000000;
}


/* Definitions for BGA. Reference Graphics 1. */
#define VBE_DISPI_IOPORT_INDEX          0x01CE
#define VBE_DISPI_IOPORT_DATA           0x01CF
#define VBE_DISPI_INDEX_XRES            0x1
#define VBE_DISPI_INDEX_YRES            0x2
#define VBE_DISPI_INDEX_BPP             0x3
#define VBE_DISPI_INDEX_ENABLE          0x4
#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_LFB_ENABLED           0x40


extern void _cdecl kKeyboardHandler();

_declspec(naked) void multiboot_entry(void)
{
	__asm {
		align 4

		multiboot_header:
		dd(MULTIBOOT_HEADER_MAGIC); magic number
			dd(MULTIBOOT_HEADER_FLAGS); flags
			dd(CHECKSUM); checksum
			dd(HEADER_ADRESS); header address
			dd(LOADBASE); load address
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

extern volatile uint32_t	_pit_ticks;

void main(unsigned long magic, unsigned long addr)
//int _cdecl kmain(multiboot_info* bootinfo) 
{
	InitializeConstructors();
	SkyConsole::Initialize();

	EnterCriticalSection();

	multiboot_info* pBootInfo = (multiboot_info*)addr;	
	
	i86_gdt_initialize();			
	i86_idt_initialize(0x8);		
	i86_pic_initialize(0x20, 0x28);
	i86_pit_initialize();
											
	SetInterruptVector();
	//setvect(32, scheduler_isr, 0x80);
		
	//커널사이즈에 512를 곱하면 바이트로 환산된다.
	//현재 커널의 사이즈는 4096바이트다.
	
	InitializeMemorySystem(pBootInfo, 0);		
	//

	//EnterCriticalSection();

	kkybrd_install(33);

	//InitializeFloppyDrive();

	InitializeSysCall();

	

	/*setvect(33, kKeyboardHandler);




	// 키보드를 활성화
	if (kInitializeKeyboard() == TRUE)
	{
		kChangeKeyboardLED(FALSE, FALSE, FALSE);
	}*/
	//i86_install_ir(SYSTEM_TMR_INT_NUMBER, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | 0x0500, 0x8, (I86_IRQ_HANDLER)TMR_TSS_SEG);
	
	
	SkyConsole::Print("Sky OS Console System Initialize\n");
	
	i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	LeaveCriticalSection();
	
	for (int i = 0; i < 100; i++)
	{
		ZetPlane* pZetPlane = new ZetPlane();
		pZetPlane->m_rotation = 50;
		SkyConsole::Print("ZetPlane Address : 0x%x\n", pZetPlane);
		SkyConsole::Print("ZetPlane m_rotation : %d\n", pZetPlane->m_rotation);
	}

	/*VbeBochsSetMode(WIDTH, HEIGHT, BPP);
	VbeBochsMapLFB();
	fillScreen32();*/

	CreateCentralSystem();
	
	/*SkyConsole::Print("Press Any Key\n");
	SkyConsole::GetChar();
	NativeConsole();*/

	/*for (;;);
	for (int i = 0;; i++)
		SkyConsole::Print("rrrr : %d\n", _pit_ticks);
	*/
	/*char	commandBuffer[MAXPATH];
	//SkyConsole::Clear();
	
	memset(commandBuffer, 0, MAXPATH);
	while (1)
	{
		SkyConsole::Print("Command> ");
		SkyConsole::GetCommand(commandBuffer, MAXPATH - 2);
		SkyConsole::Print("\n");
	}
	for (;;);*/
	
	//! install the keyboard to IR 33, uses IRQ 1
	

	
	/*setvect(33, kKeyboardHandler);
	
	// 키보드를 활성화
	if (kInitializeKeyboard() == TRUE)
	{		
		kChangeKeyboardLED(FALSE, FALSE, FALSE);
	}*/

	

	

	InitializeSysCall();

	//! initialize TSS
	install_tss(5, 0x10, 0);

	/* set video mode and map framebuffer. */
	VbeBochsSetMode(WIDTH, HEIGHT, BPP);
	VbeBochsMapLFB();
	fillScreen32();

	//HardDiskHandler hardHandler;
	//hardHandler.Initialize();		

	SkyConsole::kGetCh();
	
	SkyConsole::SetBackColor(ConsoleColor::Blue);
	SkyConsole::Clear();

	/*if (kInitializeHDD() == TRUE)
	{
		SkyConsole::Print("AAAAAAAAAAAAAAAAAAAAAAAAA\n");
	}*/

	//SkyConsole::Print("HardDisk Count : %d\n", hardHandler.GetTotalDevices());
	SkyConsole::Print("Orange OS Console System Initialize\n");
	//SkyConsole::Print("KernelSize : %d Bytes\n", g_kernelSize);	

	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.	

	char str[256];
	memset(str, 0, 256);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	sprintf(str, "LocalTime : %d %d/%d %d:%d %d\n", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	SkyConsole::Print("%s", str);

	//파라메터 kernelSize는 스택에 존재한다. stack의 베이스 주소는 0x9000이다.
	//sprintf(str, "Parameter kernelSize Variable Address : %x\n", (unsigned int)&g_kernelSize);
	SkyConsole::Print("%s", str);

	

	CreateCentralSystem();	

	//return 0;
}

__declspec(naked) void  _cdecl HandleTimerInterrupt()
{
	_asm {
		pushad
	}
	
	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		iretd
	}
}

void SetInterruptVector()
{
	setvect(0, (void(__cdecl &)(void))divide_by_zero_fault);
	setvect(1, (void(__cdecl &)(void))single_step_trap);
	setvect(2, (void(__cdecl &)(void))nmi_trap);
	setvect(3, (void(__cdecl &)(void))breakpoint_trap);
	setvect(4, (void(__cdecl &)(void))overflow_trap);
	setvect(5, (void(__cdecl &)(void))bounds_check_fault);
	setvect(6, (void(__cdecl &)(void))invalid_opcode_fault);
	setvect(7, (void(__cdecl &)(void))no_device_fault);
	setvect(8, (void(__cdecl &)(void))double_fault_abort);
	setvect(10, (void(__cdecl &)(void))invalid_tss_fault);
	setvect(11, (void(__cdecl &)(void))no_segment_fault);
	setvect(12, (void(__cdecl &)(void))stack_fault);
	setvect(13, (void(__cdecl &)(void))general_protection_fault);
	setvect(14, (void(__cdecl &)(void))page_fault);
	setvect(16, (void(__cdecl &)(void))fpu_fault);
	setvect(17, (void(__cdecl &)(void))alignment_check_fault);
	setvect(18, (void(__cdecl &)(void))machine_check_abort);
	setvect(19, (void(__cdecl &)(void))simd_fpu_fault);
	
	setvect(33, (void(__cdecl &)(void))HandleTimerInterrupt);
	setvect(38, (void(__cdecl &)(void))HandleTimerInterrupt);

	//i86_install_ir(SYSTEM_TMR_INT_NUMBER, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | 0x0500, 0x8, (I86_IRQ_HANDLER)TMR_TSS_SEG);
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
		//DebugPrintf("\nMemory Length : %x", entry->len);
		//DebugPrintf("\nMemory Type : %x", entry->type);
		//DebugPrintf("\nEntry Size : %d", entry->size);

		mmapAddr += sizeof(multiboot_memory_map_t);
		memorySize += entry->len;
	}

	return memorySize;
}

// where the kernel is to be loaded to in protected mode
// define IMAGE_PMODE_BASE 0xC0000000; 0x100000
//  where the kernel is to be loaded to in real mode
//  define IMAGE_RMODE_BASE 0x3000

bool InitializeMemorySystem(multiboot_info* bootinfo, uint32_t kernelSize)
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

void InitializeFloppyDrive()
{
	//! set drive 0 as current drive
	flpydsk_set_working_drive(0);

	//! install floppy disk to IR 38, uses IRQ 6
	flpydsk_install(38);

	//! initialize FAT12 filesystem
	fsysFatInitialize();
}

/* render rectangle in 32 bpp modes. */
extern void rect32(int x, int y, int w, int h, int col);


DWORD WINAPI RectGenerate(LPVOID parameter)
{
	int col = 0;
	bool dir = true;
	SkyConsole::Print("RectGenerate\n");
	while (1) {
		rect32(200, 250, 100, 100, col << 16);
		if (dir) {
			if (col++ == 0xfe)
				dir = false;
		}
		else
			if (col-- == 1)
				dir = true;
	}

	return 0;
}

extern bool systemOn;

void CreateCentralSystem()
{		
	Process* pProcess = ProcessManager::GetInstance()->CreateProcess(SystemEntry, true);

	if (pProcess)
		SkyConsole::Print("Create Success System Process\n");

	//Thread* newThread = ProcessManager::CreateThread(pProcess, SampleLoop, pProcess);
	//Thread* newThread2 = ProcessManager::CreateThread(pProcess, TaskProcessor, pProcess);
	
	Thread* pThread = pProcess->GetThread(0);
	pThread->m_taskState = TASK_STATE_RUNNING;

	//CreateTestKernelProcess();

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

void CreateTestKernelProcess()
{
	Process* pProcess = ProcessManager::GetInstance()->CreateProcess(TestKernelProcess);

	if (pProcess)
		SkyConsole::Print("Create Success System Process\n");	
}




