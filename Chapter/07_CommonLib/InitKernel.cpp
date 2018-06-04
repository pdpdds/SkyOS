#include "InitKernel.h"
#include "Hal.h"
#include "Exception.h"
#include "IDT.h"

void SetInterruptVector()
{
	setvect(0, (void(__cdecl &)(void))kHandleDivideByZero);
	setvect(1, (void(__cdecl &)(void))kHandleSingleStepTrap);
	setvect(2, (void(__cdecl &)(void))kHandleNMITrap);
	setvect(3, (void(__cdecl &)(void))kHandleBreakPointTrap);
	setvect(4, (void(__cdecl &)(void))kHandleOverflowTrap);
	setvect(5, (void(__cdecl &)(void))kHandleBoundsCheckFault);
	setvect(6, (void(__cdecl &)(void))kHandleInvalidOpcodeFault);
	setvect(7, (void(__cdecl &)(void))kHandleNoDeviceFault);
	setvect(8, (void(__cdecl &)(void))kHandleDoubleFaultAbort);
	setvect(10, (void(__cdecl &)(void))kHandleInvalidTSSFault);
	setvect(11, (void(__cdecl &)(void))kHandleSegmentFault);
	setvect(12, (void(__cdecl &)(void))kHandleStackFault);
	setvect(13, (void(__cdecl &)(void))kHandleGeneralProtectionFault);
	setvect(14, (void(__cdecl &)(void))kHandlePageFault);
	setvect(16, (void(__cdecl &)(void))kHandlefpu_fault);
	setvect(17, (void(__cdecl &)(void))kHandleAlignedCheckFault);
	setvect(18, (void(__cdecl &)(void))kHandleMachineCheckAbort);
	setvect(19, (void(__cdecl &)(void))kHandleSIMDFPUFault);

	setvect(33, (void(__cdecl &)(void))InterrputDefaultHandler);
	setvect(38, (void(__cdecl &)(void))InterrputDefaultHandler);
}