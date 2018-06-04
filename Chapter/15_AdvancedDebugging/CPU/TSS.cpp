#include "SkyOS.h"

tss_entry TSS;

void FlushTSS(uint16_t sel)
{
	kEnterCriticalSection();
	_asm {		
		mov eax, 0x2b
		ltr ax		
	}
	kLeaveCriticalSection();

	//_asm ltr [sel]
}

void SetTSSStack(uint16_t kernelSS, uint16_t kernelESP) {

	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
}

void InstallPageDirectory(void* pPageDirectory)
{
	TSS.ldt = (uint32_t)pPageDirectory;
}

void InstallTSS(uint32_t idx, uint16_t kernelSS, uint16_t kernelESP)
{	
	uint32_t base = (uint32_t)&TSS;
	
	gdt_set_descriptor(idx, base, base + sizeof(tss_entry),
		I86_GDT_DESC_ACCESS | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_DPL | I86_GDT_DESC_MEMORY,
		0);

	memset((void*)&TSS, 0, sizeof(tss_entry));

	//! set stack and segments
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
	TSS.cs = 0x0b;
	TSS.ss = 0x13;
	TSS.es = 0x13;
	TSS.ds = 0x13;
	TSS.fs = 0x13;
	TSS.gs = 0x13;
	
	FlushTSS((uint16_t)idx * sizeof(gdt_descriptor));
}