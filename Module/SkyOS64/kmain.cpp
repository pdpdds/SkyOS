#include "kmain.h"
#include "SkyConsole.h"
#include "MultiBoot.h"

void kmain64(int addr)
{
	InitializeConstructors();

	multiboot_info* pBootInfo = (multiboot_info*)addr;

	SkyConsole::Initialize();
	SkyConsole::Print("SkyOS64 Kernel Entered..\n");

	SkyConsole::Print("GRUB Information\n");
	SkyConsole::Print("Boot Loader Name : %s\n", pBootInfo->boot_loader_name);

	SkyConsole::Print("Hello World64\n");
	
	for (;;);
}

