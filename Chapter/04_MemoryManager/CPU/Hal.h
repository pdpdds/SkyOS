#pragma once
#include "windef.h"

#ifdef _MSC_VER
#define interrupt __declspec (naked)
#else
#define interrupt
#endif

#define PAGE_SIZE 4096

#define PAGE_ALIGN_DOWN(value)				((value) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(value)				(((value) & (PAGE_SIZE - 1)) ? (PAGE_ALIGN_DOWN((value)) + PAGE_SIZE) : (value))

#define far
#define near

extern "C" int _outp(unsigned short, int);
extern "C" unsigned long _outpd(unsigned int, int);
extern "C" unsigned short _outpw(unsigned short, unsigned short);
extern "C" int _inp(unsigned short);
extern "C" unsigned short _inpw(unsigned short);
extern "C" unsigned long _inpd(unsigned int shor);

void OutPortByte(ushort port, uchar value);
void OutPortWord(ushort port, ushort value);
void OutPortDWord(ushort port, unsigned int value);
uchar InPortByte(ushort port);
ushort InPortWord(ushort port);
long InPortDWord(unsigned int port);

void InterruptDone(unsigned int intno);
void setvect(int intno, void(&vect) (), int flags = 0);

#pragma pack (push, 1)
typedef struct registers
{
	u32int ds, es, fs, gs;                  // Data segment selector
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pushad.
	//u32int int_no, err_code;    // Interrupt number and error code (if applicable)
	u32int eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;
#pragma pack (pop)

extern void _cdecl msleep(int ms);
