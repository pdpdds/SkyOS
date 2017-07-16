#include "kernel.h"
#include "DebugDisplay.h"
#include "CRTInitialze.h"
#include "SkyHal.h"

__declspec(naked) void multiboot_entry(void)
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

void TestExceptionHandler()
{
	int a = 100;
	a = a / 0;
}

void main(unsigned long magic, unsigned long addr)
{
	InitializeConstructors();

	DebugClrScr(0x13);
	DebugGotoXY(0, 0);
	DebugSetColor(0x17);

	DebugPrintf("SkyOS Initializing...");	

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
	{
		multiboot_info* pBootInfo = (multiboot_info*)addr;

		uint32_t mmapEntryNum = pBootInfo->mmap_length / sizeof(multiboot_memory_map_t);
		
		DebugPrintf("\nGRUB Impormation");
		DebugPrintf("\nBoot Loader Name : %s", (char*)pBootInfo->boot_loader_name);
		DebugPrintf("\nMemory Map Entry Num : %d", mmapEntryNum);

		uint32_t mmapAddr = pBootInfo->mmap_addr;
		
		for (int i = 0; i < mmapEntryNum; i++)
		{
			multiboot_memory_map_t* entry = (multiboot_memory_map_t*)mmapAddr;

			DebugPrintf("\nMemory Address : %x", entry->addr);
			DebugPrintf("\nMemory Length : %x", entry->len);
			DebugPrintf("\nMemory Type : %x", entry->type);
			DebugPrintf("\Entry Size : %d", entry->size);
						
			mmapAddr += sizeof(multiboot_memory_map_t);
		}		
	}

	if(true == SetupGDT())
		DebugPrintf("\nSetup GDT Success");

	if (true == SetupIDT())
		DebugPrintf("\nSetup IDT Success");

	if(true == SetupInterruptHandlers())
		DebugPrintf("\nSetup Exception Handler Success");

	//TestExceptionHandler();

	if (true == SetupPIC())
		DebugPrintf("\nSetup PIC Handler Success");

	if (true == SetupPIT())
		DebugPrintf("\nSetup PIT Handler Success");	

	//_asm sti

	for (;;);
}
