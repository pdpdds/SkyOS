#include "Graphics.h"
#include "SkyConsole.h"
#include "string.h"
#include "stdio.h"
#include "exception.h"
#include "VirtualMemoryManager.h"
#include "sprintf.h"

uint16_t bochs_resolution_x = 0;
uint16_t bochs_resolution_y = 0;
uint16_t bochs_resolution_b = 0;
uint8_t * bochs_vid_memory = (uint8_t *)0xE0000000;


void finalize_graphics(uint16_t x, uint16_t y, uint16_t b) {
	bochs_resolution_x = x;
	bochs_resolution_y = y;
	bochs_resolution_b = b;
}

void rect32A(int x, int y, int w, int h, int col) {
	int* lfb = (int*)bochs_vid_memory;
	for (int k = 0; k < h; k++)
		for (int j = 0; j < w; j++)
			lfb[(j + x) + (k + y) * w] = col;
}

void graphics_install_vesa(uint16_t resX, uint16_t resY)
{
	/* VESA Structs */
	struct VesaControllerInfo *info = (VesaControllerInfo*)0x10000;
	struct VesaModeInfo *modeinfo = (VesaModeInfo*)0x9000;

	/* 8086 Emulator Status */
	tRME_State *emu;
	void * lowCache;
	lowCache = (void *)new BYTE[RME_BLOCK_SIZE];
	memcpy(lowCache, NULL, RME_BLOCK_SIZE);
	emu = RME_CreateState();
	emu->Memory[0] = (uint8_t*)lowCache;
	for (int i = RME_BLOCK_SIZE; i < 0x100000; i += RME_BLOCK_SIZE) {
		emu->Memory[i / RME_BLOCK_SIZE] = (uint8_t*)i;
	}
	int ret, mode;

	/* Find modes */
	uint16_t * modes;
	memcpy(info->Signature, "VBE2", 4);
	emu->AX.W = 0x4F00;
	emu->ES = 0x1000;
	emu->DI.W = 0;
	ret = RME_CallInt(emu, 0x10);
	if (info->Version < 0x200 || info->Version > 0x300)
	{
		SkyConsole::Print("\033[JYou have attempted to use the VESA/VBE2 driver\nwith a card that does not support VBE2.\n");
		SkyConsole::Print("\nSystem responded to VBE request with version: 0x%x\n", info->Version);
		char errMsg[256];
		sprintf(errMsg, "System responded to VBE request with version: 0x%x\n", info->Version);
		HaltSystem("errMsg");
	}
	modes = (uint16_t*)FP_TO_LINEAR(info->Videomodes.Segment, info->Videomodes.Offset);

	uint16_t best_x = 0;
	uint16_t best_y = 0;
	uint16_t best_b = 0;
	uint16_t best_mode = 0;

	for (int i = 1; modes[i] != 0xFFFF; ++i) {
		emu->AX.W = 0x4F01;
		emu->CX.W = modes[i];
		emu->ES = 0x0900;
		emu->DI.W = 0x0000;
		RME_CallInt(emu, 0x10);
#if PROMPT_FOR_MODE
		SkyConsole::Print("%d = %dx%d:%d %d\n", i, modeinfo->Xres, modeinfo->Yres, modeinfo->bpp, modeinfo->physbase);
	}

	SkyConsole::Print("Please select a mode: ");
	char selected = SkyConsole::GetChar();
	char buf[10];
	buf[0] = selected;
	buf[1] = '\n';

	mode = atoi(buf);
#else
		if ((abs(modeinfo->Xres - resX) < abs(best_x - resX)) && (abs(modeinfo->Yres - resY) < abs(best_y - resY))) {
			best_mode = i;
			best_x = modeinfo->Xres;
			best_y = modeinfo->Yres;
			best_b = modeinfo->bpp;
		}
}
	for (int i = 1; modes[i] != 0xFFFF; ++i) {
		emu->AX.W = 0x4F01;
		emu->CX.W = modes[i];
		emu->ES = 0x0900;
		emu->DI.W = 0x0000;
		RME_CallInt(emu, 0x10);
		if (modeinfo->Xres == best_x && modeinfo->Yres == best_y) {
			if (modeinfo->bpp > best_b) {
				best_mode = i;
				best_b = modeinfo->bpp;
			}
		}
	}

	if (best_b < 24) {
		SkyConsole::Print("!!! Rendering at this bit depth (%d) is not currently supported.\n", best_b);
		HaltSystem("");
	}

	mode = best_mode;

#endif

	emu->AX.W = 0x4F01;
	if (mode < 100) {
		emu->CX.W = modes[mode];
	}
	else {
		emu->CX.W = mode;
	}
	emu->ES = 0x0900;
	emu->DI.W = 0x0000;
	RME_CallInt(emu, 0x10);

	

	emu->AX.W = 0x4F02;
	emu->BX.W = modes[mode];

	RME_CallInt(emu, 0x10);
	
	uint16_t actual_x = modeinfo->Xres;
	uint16_t actual_y = modeinfo->Yres;
	uint16_t actual_b = modeinfo->bpp;

	bochs_vid_memory = (uint8_t *)modeinfo->physbase;

	if (!bochs_vid_memory) {
		uint32_t * herp = (uint32_t *)0xA0000;
		herp[0] = 0xA5ADFACE;
		
		//memset(bochs_vid_memory, 0xff, modeinfo->Xres * modeinfo->Yres);

			
		/* Enable the higher memory */
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(0xE0000000, 0xE0FF0000);
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(0xF0000000, 0xF0FF0000);
			
		/* Go find it */
		for (uintptr_t x = 0xE0000000; x < 0xE0FF0000; x += 0x1000) {
			if (((uintptr_t *)x)[0] == 0xA5ADFACE) {
				bochs_vid_memory = (uint8_t *)x;

				
				goto mem_found;
			}
		}
		for (uintptr_t x = 0xF0000000; x < 0xF0FF0000; x += 0x1000)
		{
			if (((uintptr_t *)x)[0] == 0xA5ADFACE) {
				bochs_vid_memory = (uint8_t *)x;


				SkyConsole::GetChar();
				SkyConsole::GetChar();

				rect32A(100, 100, 200, 200, 0xAAAAAAAA);
		
				goto mem_found;
			}
		}

		
	}
mem_found:	
	/*
	* Finalize the graphics setup with the actual selected resolution.
	*/
	finalize_graphics(actual_x, actual_y, actual_b);	

	//InitGraphics(modeinfo);
}




void TestV8086()
{
	graphics_install_vesa(1024, 768);



	//HaltSystem("ff");

	int col = 0;
	bool dir = true;
	SkyConsole::Print("RectGenerate\n");
	while (1) {
		rect32A(200, 380, 100, 100, 0x80);
		if (dir) {
			if (col++ == 0xfe)
				dir = false;
		}
		else
			if (col-- == 1)
				dir = true;
	}
}

