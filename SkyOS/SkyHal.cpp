#include "SkyHal.h"
#include "SkyGDT.h"
#include "SkyIDT.h"
#include "SkyPIT.h"
#include "SkyPIC.h"
#include "InterruptHandler.h"

bool SetupGDT()
{
	return InitializeGDT();
}

bool SetupIDT()
{
	return InitializeIDT(0x08);
}

bool SetupInterruptHandlers()
{	
	SetupInterruptHandler(0, (void(__cdecl &)(void))divide_by_zero_fault);
	SetupInterruptHandler(1, (void(__cdecl &)(void))single_step_trap);
	SetupInterruptHandler(2, (void(__cdecl &)(void))nmi_trap);
	SetupInterruptHandler(3, (void(__cdecl &)(void))breakpoint_trap);
	SetupInterruptHandler(4, (void(__cdecl &)(void))overflow_trap);
	SetupInterruptHandler(5, (void(__cdecl &)(void))bounds_check_fault);
	SetupInterruptHandler(6, (void(__cdecl &)(void))invalid_opcode_fault);
	SetupInterruptHandler(7, (void(__cdecl &)(void))no_device_fault);
	SetupInterruptHandler(8, (void(__cdecl &)(void))double_fault_abort);
	SetupInterruptHandler(10, (void(__cdecl &)(void))invalid_tss_fault);
	SetupInterruptHandler(11, (void(__cdecl &)(void))no_segment_fault);
	SetupInterruptHandler(12, (void(__cdecl &)(void))stack_fault);
	SetupInterruptHandler(13, (void(__cdecl &)(void))general_protection_fault);
	SetupInterruptHandler(14, (void(__cdecl &)(void))page_fault);
	SetupInterruptHandler(16, (void(__cdecl &)(void))fpu_fault);
	SetupInterruptHandler(17, (void(__cdecl &)(void))alignment_check_fault);
	SetupInterruptHandler(18, (void(__cdecl &)(void))machine_check_abort);
	SetupInterruptHandler(19, (void(__cdecl &)(void))simd_fpu_fault);

	return true;
}

bool SetupPIT()
{
	SetupInterruptHandler(32, (void(__cdecl &)(void))HandleTimerInterrupt);
	i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	return true;
}

bool SetupPIC()
{
	i86_pic_initialize(0x20, 0x28);
	return true;
}