#pragma once
#include "SkyHal.h"
#include "StdIntTypes.h"

void _cdecl SetupInterruptHandler(int intno, void(_cdecl &hander) (), int flags = 0);

void _cdecl divide_by_zero_fault(uint32_t eflags, uint32_t cs, uint32_t eip, uint32_t other);
void _cdecl single_step_trap(unsigned int cs, unsigned int eip, unsigned int eflags);
void _cdecl nmi_trap(unsigned int cs, unsigned int eip, unsigned int eflags);
void _cdecl breakpoint_trap(unsigned int cs, unsigned int eip, unsigned int eflags);
void _cdecl overflow_trap(unsigned int cs, unsigned int eip, unsigned int eflags);
void _cdecl bounds_check_fault(unsigned int cs, unsigned int eip, unsigned int eflags);
void _cdecl invalid_opcode_fault(unsigned int cs, unsigned int eip, unsigned int eflags);
void _cdecl no_device_fault(unsigned int cs, unsigned int eip, unsigned int eflags);
void _cdecl double_fault_abort(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
void _cdecl invalid_tss_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
void _cdecl no_segment_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
void _cdecl stack_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
void _cdecl general_protection_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
void _cdecl page_fault(uint32_t err, uint32_t eflags, uint32_t cs, uint32_t eip);
void _cdecl fpu_fault(unsigned int cs, unsigned int eip, unsigned int eflags);
void _cdecl alignment_check_fault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);
void _cdecl machine_check_abort(unsigned int cs, unsigned int eip, unsigned int eflags);
void _cdecl simd_fpu_fault(unsigned int cs, unsigned int eip, unsigned int eflags);