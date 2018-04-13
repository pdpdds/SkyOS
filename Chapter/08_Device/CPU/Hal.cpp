#include "Hal.h"
#include "Pic.h"
#include "IDT.h"
#include "SkyAPI.h"

void OutPortByte(ushort port, uchar value)
{
	_outp(port, value);
}

void OutPortWord(ushort port, ushort value)
{
	_outpw(port, value);
}

void OutPortDWord(ushort port, unsigned int value)
{
	_outpd(port, value);
}

long InPortDWord(unsigned int port)
{
	return _inpd(port);
}

uchar InPortByte(ushort port)
{
#ifdef _MSC_VER
	_asm {
		mov		dx, word ptr[port]
		in		al, dx
		mov		byte ptr[port], al
	}
#endif
	return (unsigned char)port;
	//return (uchar)_inp(port);
}

ushort InPortWord(ushort port)
{
	return _inpw(port);
}

/*void OutPortByte(ushort port, uchar value)
{
#ifdef _MSC_VER
	_asm {
		mov		al, byte ptr[value]
		mov		dx, word ptr[port]
		out		dx, al
	}
#endif
}

void OutPortWord(ushort port, ushort value)
{
	_outpw(port, value);
}

uchar InPortByte(ushort port)
{
#ifdef _MSC_VER
	_asm {
		mov		dx, word ptr[port]
		in		al, dx
		mov		byte ptr[port], al
	}
#endif
	return (unsigned char)port;
}

ushort InPortWord(ushort port)
{
	return _inpw(port);
}*/

//! notifies hal interrupt is done
void InterruptDone(unsigned int intno) {

	//! insure its a valid hardware irq
	if (intno > 16)
		return;

	//! test if we need to send end-of-interrupt to second pic
	if (intno >= 8)
		SendCommandToPIC(I86_PIC_OCW2_MASK_EOI, 1);

	//! always send end-of-interrupt to primary pic
	SendCommandToPIC(I86_PIC_OCW2_MASK_EOI, 0);
}

//! sets new interrupt vector
void setvect(int intno, void(&vect) (), int flags) {

	//! install interrupt handler! This overwrites prev interrupt descriptor
	InstallInterrputHandler(intno, (uint16_t)(I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | flags), 0x8, vect);
}

DWORD kReadFLAGS()
{
	UINT flag = 0;
	__asm
	{
		pushf; RFLAGS 레지스터를 스택에 저장
		pop eax; 스택에 저장된 RFLAGS 레지스터를 RAX 레지스터에 저장하여		
		mov [flag], eax
	}

	return flag;
}

bool kSetInterruptFlag(bool bEnableInterrupt)
{
	DWORD dwRFLAGS;

	// 이전의 RFLAGS 레지스터 값을 읽은 뒤에 인터럽트 가능/불가 처리
	dwRFLAGS = kReadFLAGS();
	if (bEnableInterrupt == TRUE)
	{
		kLeaveCriticalSection(&g_criticalSection);
	}
	else
	{
		kEnterCriticalSection(&g_criticalSection);
	}

	// 이전 RFLAGS 레지스터의 IF 비트(비트 9)를 확인하여 이전의 인터럽트 상태를 반환
	if (dwRFLAGS & 0x0200)
	{
		return TRUE;
	}
	return FALSE;
}