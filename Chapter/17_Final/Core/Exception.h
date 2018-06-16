#pragma once
#include "stdint.h"

void kHandleDivideByZero();

//싱글 스텝
void kHandleSingleStepTrap();

// non maskable interrupt trap
void kHandleNMITrap();

//브레이크 포인트
void kHandleBreakPointTrap();

//오버플로우
void kHandleOverflowTrap();

// bounds check
void kHandleBoundsCheckFault();

//유효하지 않은 OPCODE 또는 명령어
void kHandleInvalidOpcodeFault();

//디바이스 이용할 수 없음
void kHandleNoDeviceFault ();

//더블 폴트
void kHandleDoubleFaultAbort ();

//유효하지 않은 TSS(Task State Segment)
void kHandleInvalidTSSFault ();

//세그먼트 존재하지 않음
void kHandleSegmentFault ();

//스택 폴트
void kHandleStackFault();

//일반 보호 오류(General Protection Fault)
void kHandleGeneralProtectionFault ();

//페이지 폴트
void kHandlePageFault ();

// Floating Point Unit (FPU) error
void kHandlefpu_fault ();

//alignment check
void kHandleAlignedCheckFault();

//machine check
void kHandleMachineCheckAbort ();

//Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
void kHandleSIMDFPUFault();

void HaltSystem(const char* errMsg);