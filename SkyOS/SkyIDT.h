#pragma once
#include "StdIntTypes.h"

//! i86 defines 256 possible interrupt handlers (0-255)
#define I86_MAX_INTERRUPTS		256

//! must be in the format 0D110, where D is descriptor type
#define I86_IDT_DESC_BIT16		0x06	//00000110
#define I86_IDT_DESC_BIT32		0x0E	//00001110
#define I86_IDT_DESC_RING1		0x40	//01000000
#define I86_IDT_DESC_RING2		0x20	//00100000
#define I86_IDT_DESC_RING3		0x60	//01100000
#define I86_IDT_DESC_PRESENT		0x80	//10000000

//! interrupt handler w/o error code
//! Note: interrupt handlers are called by the processor. The stack setup may change
//! so we leave it up to the interrupts' implimentation to handle it and properly return
typedef void(_cdecl *I86_IRQ_HANDLER)(void);

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

//! interrupt descriptor
struct idt_descriptor {

	//! bits 0-16 of interrupt routine (ir) address
	uint16_t		baseLo;

	//! code selector in gdt
	uint16_t		sel;

	//! reserved, shold be 0
	uint8_t			reserved;

	//! bit flags. Set with flags above
	uint8_t			flags;

	//! bits 16-32 of ir address
	uint16_t		baseHi;
};

#ifdef _MSC_VER
#pragma pack (pop)
#endif

idt_descriptor* GetInterrputDescriptor(uint32_t i);
bool InstallIDTInterrupt(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER);
bool InitializeIDT(uint16_t codeSel);
