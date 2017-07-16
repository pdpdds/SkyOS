#include "InterruptHandler.h"
#include "SkyIDT.h"
#include "DebugDisplay.h"
#include "stdio.h"
#include "SkyString.h"

static char* sickpc = " \
                               _______      \n\
                               |.-----.|    \n\
                               ||x . x||    \n\
                               ||_.-._||    \n\
                               `--)-(--`    \n\
                              __[=== o]___  \n\
                             |:::::::::::|\\ \n\
                             `-=========-`()\n\
                                M. O. S.\n\n";

//! something is wrong--bail out
void _cdecl kernel_panic(const char* fmt, ...) {

//	disable();

	va_list		args;
	static char buf[1024];

	va_start(args, fmt);
	size_t i;
	for (i = 0; i<strlen(fmt); i++) {

		switch (fmt[i]) {

		case '%':

			switch (fmt[i + 1]) {

				/*** characters ***/
			case 'c': {
				char c = va_arg(args, char);
				DebugPutc(c);
				i++;		// go to next character
				break;
			}

					  /*** address of ***/
			case 's': {
				int c = (int&)va_arg(args, char);
				char str[64];
				strcpy(str, (const char*)c);
				DebugPuts(str);
				i++;		// go to next character
				break;
			}

					  /*** integers ***/
			case 'd':
			case 'i': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 10, str);
				DebugPuts(str);
				i++;		// go to next character
				break;
			}

					  /*** display in hex ***/
			case 'X':
			case 'x': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				DebugPuts(str);
				i++;		// go to next character
				break;
			}

			default:
				break;
			}

			

		default:
			DebugPutc(fmt[i]);
			break;
		}

	}
	va_end(args);

	char* disclamer = "We apologize, MOS has encountered a problem and has been shut down\n\
to prevent damage to your computer. Any unsaved work might be lost.\n\
We are sorry for the inconvenience this might have caused.\n\n\
Please report the following information and restart your computer.\n\
The system has been halted.\n\n";

	DebugClrScr(0x1f);
	DebugGotoXY(0, 0);
	DebugSetColor(0x1f);
	DebugPuts(sickpc);
	DebugPuts(disclamer);

	DebugPrintf("*** STOP: %s", fmt);

	for (;;);
}

void _cdecl SetupInterruptHandler(int intno, void(_cdecl &hander) (), int flags)
{

	//! install interrupt handler! This overwrites prev interrupt descriptor
	InstallIDTInterrupt((unsigned int)intno, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | (uint16_t)flags, 0x8, hander);
}

extern void _cdecl kernel_panic(const char* fmt, ...);

#define intstart() \
	_asm	cli \
	_asm	sub		ebp, 4


#pragma warning (disable:4100)

//! divide by 0 fault
void interrupt _cdecl divide_by_zero_fault(uint32_t eflags, uint32_t cs, uint32_t eip, uint32_t other) {

	_asm {
		cli
		add esp, 12
		pushad
	}

	DebugClrScr(0x1f);
	DebugGotoXY(0, 0);
	DebugSetColor(0x1f);
	DebugPuts(sickpc);
	//DebugPuts(disclamer);

	DebugPrintf("Divide by 0 at physical address [0x%x:0x%x] EFLAGS [0x%x] other: 0x%x", cs, eip, eflags, other);	
	for (;;);
}

//! single step
void interrupt _cdecl single_step_trap(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Single step");
	for (;;);
}

//! non maskable interrupt trap
void interrupt _cdecl nmi_trap(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("NMI trap");
	for (;;);
}

//! breakpoint hit
void interrupt _cdecl breakpoint_trap(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Breakpoint trap");
	for (;;);
}

//! overflow
void interrupt _cdecl overflow_trap(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Overflow trap");
	for (;;);
}

//! bounds check
void interrupt _cdecl bounds_check_fault(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Bounds check fault");
	for (;;);
}

//! invalid opcode / instruction
void interrupt _cdecl invalid_opcode_fault(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Invalid opcode");
	for (;;);
}

//! device not available
void interrupt _cdecl no_device_fault(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Device not found");
	for (;;);
}

//! double fault
void interrupt _cdecl double_fault_abort(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Double fault");
	for (;;);
}

//! invalid Task State Segment (TSS)
void interrupt _cdecl invalid_tss_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Invalid TSS");
	for (;;);
}

//! segment not present
void interrupt _cdecl no_segment_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Invalid segment");
	for (;;);
}

//! stack fault
void interrupt _cdecl stack_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Stack fault");
	for (;;);
}

//! general protection fault
void interrupt _cdecl general_protection_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("General Protection Fault");
	for (;;);
}


//! page fault
void _cdecl page_fault(uint32_t err, uint32_t eflags, uint32_t cs, uint32_t eip) {

	_asm	cli
	_asm	sub		ebp, 4

	//	int faultAddr=0;

	//	_asm {
	//		mov eax, cr2
	//		mov [faultAddr], eax
	//	}

	//	kernel_panic ("Page Fault at 0x%x:0x%x refrenced memory at 0x%x",
	//		cs, eip, faultAddr);
	//	kernel_panic ("Page Fault");
	for (;;);
	_asm popad
	_asm sti
	_asm iretd
}

//! Floating Point Unit (FPU) error
void interrupt _cdecl fpu_fault(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("FPU Fault");
	for (;;);
}

//! alignment check
void interrupt _cdecl alignment_check_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Alignment Check");
	for (;;);
}

//! machine check
void interrupt _cdecl machine_check_abort(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("Machine Check");
	for (;;);
}

//! Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
void interrupt _cdecl simd_fpu_fault(unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart();
	kernel_panic("FPU SIMD fault");
	for (;;);
}
