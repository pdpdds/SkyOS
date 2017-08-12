
#include "Console.h"
#include "stdint.h"
#include "MultiBoot.h"

uint32_t g_kernelSize = 0;

//! basic crt initilization stuff
extern void __cdecl  InitializeConstructors();
extern void __cdecl  Exit();

//! main
extern void kmain(unsigned long magic, unsigned long addr);

//! kernel entry point is called by boot loader
void __cdecl  kernel_entry(multiboot_info* bootinfo) {

#ifdef ARCH_X86

	// Set registers for protected mode
	_asm {
		cli
		mov ax, 10h
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
	}
#endif //ARCH_X86
	for (;;);

	//dx 레지스터에는 커널의 크기가 담겨 있다.
	//다른값으로 씌워지기 전에 값을 얻어낸다.
	_asm	mov	word ptr[g_kernelSize], dx
	InitializeConstructors();
	kmain(MULTIBOOT_HEADER_MAGIC, (unsigned long)bootinfo);
	Exit();

#ifdef ARCH_X86
	_asm {
		cli
		hlt
	}
#endif

	for (;;);
}

