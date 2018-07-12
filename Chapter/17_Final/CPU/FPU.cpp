#include "FPU.h"

extern "C" void __writecr4(unsigned __int64 Data);
extern "C"  unsigned long __readcr4(void);

bool InitFPU()
{	
	int result = 0;
	unsigned short temp;

	__asm
	{
		pushad
		mov eax, cr0; eax = CR0
		and al, ~6; Clear the EM and MP flags(just in case)
		mov cr0, eax; Set CR0
		fninit; Reset FPU status word
		mov temp, 0x5A5A; Make sure temporary word is non - zero
		fnstsw temp; Save the FPU status word in the temporary word
		cmp temp, 0; Was the correct status written to the temporary word ?
		jne noFPU; no, no FPU present
		fnstcw temp; Save the FPU control word in the temporary word
		mov ax, temp; ax = saved FPU control word
		and ax, 0x103F; ax = bits to examine
		cmp ax, 0x003F; Are the bits to examine correct ?
		jne noFPU; no, no FPU present
		mov result, 1
		noFPU:
	
		popad
	}

	return result == 1;	
}

bool EnableFPU()
{
//#ifdef _WIN32
	unsigned long regCR4 = __readcr4();
	__asm or regCR4, 0x200
	__writecr4(regCR4);	
//#else
	//mov eax, cr4;
	//or eax, 0x200
	//mov cr4, eax
// #endif
	return true;
}


