
#ifndef _EXCEPTION_H
#define _EXCEPTION_H
//****************************************************************************
//**
//**    exception.h
//**		system exception handlers. These are registered during system
//**		initialization and called automatically when they are encountered
//**
//****************************************************************************

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================

#include <stdint.h>

//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================

//! exception handlers

//! divide by 0
//extern void _cdecl divide_by_zero_fault (uint32_t a, uint32_t b,uint32_t c);

extern void _cdecl divide_by_zero_fault (uint32_t eflags,uint32_t cs,uint32_t eip, uint32_t other);

//! single step
extern void _cdecl single_step_trap (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//! non maskable interrupt trap
extern void _cdecl nmi_trap (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//! breakpoint hit
extern void _cdecl breakpoint_trap (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//! overflow
extern void _cdecl overflow_trap (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//! bounds check
extern void _cdecl bounds_check_fault (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//! invalid opcode / instruction
extern void _cdecl invalid_opcode_fault (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//! device not available
extern void _cdecl no_device_fault (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//! double fault
extern void _cdecl double_fault_abort (unsigned int cs, unsigned int err,
                      unsigned int eip, unsigned int eflags);

//! invalid Task State Segment (TSS)
extern void _cdecl invalid_tss_fault (unsigned int cs, unsigned int err,
                      unsigned int eip, unsigned int eflags);

//! segment not present
extern void _cdecl no_segment_fault (unsigned int cs, unsigned int err,
                      unsigned int eip, unsigned int eflags);

//! stack fault
extern void _cdecl stack_fault (unsigned int cs, unsigned int err, 
                      unsigned int eip, unsigned int eflags);

//! general protection fault
extern void _cdecl general_protection_fault (unsigned int cs, unsigned int err, 
                      unsigned int eip, unsigned int eflags);

//! page fault
extern void _cdecl page_fault (unsigned int cs, unsigned int err, 
                      unsigned int eip, unsigned int eflags);

//! Floating Point Unit (FPU) error
extern void _cdecl fpu_fault (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//! alignment check
extern void _cdecl alignment_check_fault (unsigned int cs, unsigned int err,
                      unsigned int eip, unsigned int eflags);

//! machine check
extern void _cdecl machine_check_abort (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//! Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
extern void _cdecl simd_fpu_fault (unsigned int cs, 
                      unsigned int eip, unsigned int eflags);

//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END [exception.h]
//**
//****************************************************************************

#endif
