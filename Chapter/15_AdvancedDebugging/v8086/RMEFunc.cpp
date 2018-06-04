#include "SkyOS.h"
#include "RME.h"
#include "VESA.h"

/* vm86 Helpers */
typedef uint32_t  FARPTR;
typedef uintptr_t addr_t;
#define MK_FP(seg, off)        ((FARPTR) (((uint32_t) (seg) << 16) | (uint16_t) (off)))
#define FP_SEG(fp)             (((FARPTR) fp) >> 16)
#define FP_OFF(fp)             (((FARPTR) fp) & 0xffff)
#define FP_TO_LINEAR(seg, off) ((void*) ((((uint16_t) (seg)) << 4) + ((uint16_t) (off))))
#define LINEAR_TO_FP(ptr)      (MK_FP(((addr_t) (ptr) - ((addr_t) (ptr) & 0xf)) / 16, ((addr_t)(ptr) & 0xf)))

uint8_t lowCache[RME_BLOCK_SIZE];
char lowCacheBuffer[4096];

void RequesGUIResolution()
{
	bool result = false;
	/* VESA Structs */

	struct VesaControllerInfo *info = (VesaControllerInfo*)0x10000;
	struct VbeModeInfo *modeinfo = (VbeModeInfo*)0x9000;

	/* 8086 Emulator Status */
	tRME_State *emu;

	uint16_t *zeroptr = (uint16_t*)0;
	memcpy(lowCache, zeroptr, RME_BLOCK_SIZE);
	
	emu = RME_CreateState();
	emu->Memory[0] = (uint8_t*)lowCache;

	for (int i = 1; i < 0x100000 / RME_BLOCK_SIZE; i++)
		emu->Memory[i] = (uint8_t*)(i*RME_BLOCK_SIZE);
	int ret = 0, mode = 0;

	/* Find modes */
	uint16_t * modes;
	memset(info, 0, sizeof(VesaControllerInfo));
	memcpy(info->Signature, "VBE2", 4);
	emu->AX.W = 0x4F00;
	emu->ES = 0x1000;
	emu->DI.W = 0;
	ret = RME_CallInt(emu, 0x10);	

	if (info->Version < 0x200 || info->Version > 0x300)
	{
		SkyConsole::Print("You have attempted to use the VESA/VBE2 driver\nwith a card that does not support VBE2.\n");
		SkyConsole::Print("System responded to VBE request with version: 0x%x\n", info->Version);
		
		return;
	}

	SkyConsole::Print("\nSystem responded to VBE request with version: 0x%x\n", info->Version);

	modes = (uint16_t*)FP_TO_LINEAR(info->Videomodes.Segment, info->Videomodes.Offset);

	uint16_t best_x = 0;
	uint16_t best_y = 0;
	uint16_t best_b = 0;
	uint16_t best_mode = 0;

	for (int i = 1; modes[i] != 0xFFFF; ++i)
	{
		emu->AX.W = 0x4F01;
		emu->CX.W = modes[i];
		emu->ES = 0x0900;
		emu->DI.W = 0x0000;
		RME_CallInt(emu, 0x10);

		SkyConsole::Print(" %d = %dx%d:%d %d %d\n", i, modeinfo->XResolution, modeinfo->YResolution, modeinfo->BitsPerPixel, modeinfo->FrameBuffer, modes[i]);
	}

}

bool SwitchGUIMode(int xRes, int yRes, int pixel)
{

	bool result = false;
	/* VESA Structs */

	struct VesaControllerInfo *info = (VesaControllerInfo*)0x10000;
	struct VbeModeInfo *modeinfo = (VbeModeInfo*)0x9000;

	/* 8086 Emulator Status */
	tRME_State *emu;

	uint16_t *zeroptr = (uint16_t*)0;
	memcpy(lowCache, zeroptr, RME_BLOCK_SIZE);

	emu = RME_CreateState();
	emu->Memory[0] = (uint8_t*)lowCache;

	for (int i = 1; i < 0x100000 / RME_BLOCK_SIZE; i++)
		emu->Memory[i] = (uint8_t*)(i*RME_BLOCK_SIZE);
	int ret = 0, mode = 0;

	/* Find modes */
	uint16_t * modes;
	memset(info, 0, sizeof(VesaControllerInfo));
	memcpy(info->Signature, "VBE2", 4);
	emu->AX.W = 0x4F00;
	emu->ES = 0x1000;
	emu->DI.W = 0;
	ret = RME_CallInt(emu, 0x10);

	if (info->Version < 0x200 || info->Version > 0x300)
	{
		SkyConsole::Print("You have attempted to use the VESA/VBE2 driver\nwith a card that does not support VBE2.\n");
		SkyConsole::Print("System responded to VBE request with version: 0x%x\n", info->Version);

		return false;
	}

	SkyConsole::Print("\nSystem responded to VBE request with version: 0x%x\n", info->Version);

	modes = (uint16_t*)FP_TO_LINEAR(info->Videomodes.Segment, info->Videomodes.Offset);

	uint16_t best_x = 0;
	uint16_t best_y = 0;
	uint16_t best_b = 0;
	uint16_t best_mode = 0;

	for (int i = 1; modes[i] != 0xFFFF; ++i)
	{
		emu->AX.W = 0x4F01;
		emu->CX.W = modes[i];
		emu->ES = 0x0900;
		emu->DI.W = 0x0000;
		RME_CallInt(emu, 0x10);

		//SkyConsole::Print("%d = %dx%d:%d %d %x\n", i, modeinfo->XRes, modeinfo->YRes, modeinfo->BitsPerPixel, modeinfo->PhysBasePtr, modes[i]);
	}

	//특정 그래픽 모드로 전환 가능한지 확인하고 가능하면 전환하는 예제
	//0x 117 1024x768 해상도에 16비트(R(5):G(6):B(5)) 색 모드 지정
	//0x4117 1024x768 해상도에 16비트(R(5):G(6):B(5)) 색을 사용하는 선형 프레임 버퍼 모드 지정

	emu->AX.W = 0x4F01;
	emu->CX.W = 0x105;
	emu->BX.W = 0x07E0;
	emu->ES = 0x07E0;
	emu->DI.W = 0;
	ret = RME_CallInt(emu, 0x10);

	emu->AX.W = 0x4F02;
	emu->BX.W = 0x4105;
	ret = RME_CallInt(emu, 0x10);

	return true;
}

