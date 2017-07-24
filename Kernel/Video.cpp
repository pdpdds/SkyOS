#include "Video.h"
#include "VirtualMemoryManager.h"

/* writes to BGA port. */
void VbeBochsWrite(uint16_t index, uint16_t value) {
	OutPortWord(VBE_DISPI_IOPORT_INDEX, index);
	OutPortWord(VBE_DISPI_IOPORT_DATA, value);
}

/* sets video mode. */
void VbeBochsSetMode(uint16_t xres, uint16_t yres, uint16_t bpp) {
	VbeBochsWrite(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
	VbeBochsWrite(VBE_DISPI_INDEX_XRES, xres);
	VbeBochsWrite(VBE_DISPI_INDEX_YRES, yres);
	VbeBochsWrite(VBE_DISPI_INDEX_BPP, bpp);
	VbeBochsWrite(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

/* map LFB into current process address space. */
void* VbeBochsMapLFB() {
	int pfcount = WIDTH*HEIGHT*BYTES_PER_PIXEL / PAGE_SIZE;
	for (int c = 0; c <= pfcount; c++)
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(VirtualMemoryManager::GetCurPageDirectory(), LFB_VIRTUAL + c * PAGE_SIZE, LFB_PHYSICAL + c * PAGE_SIZE, 7);
	return (void*)LFB_VIRTUAL;
}

/* clear screen to white. */
void fillScreen32() {
	uint32_t* lfb = (uint32_t*)LFB_VIRTUAL;
	for (uint32_t c = 0; c<WIDTH*HEIGHT; c++)
		lfb[c] = 0x00000000;
}

