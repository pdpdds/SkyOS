#include "SkyOS.h"
#include "VESA.h"

#define RGB16_565(r,g,b) ((b&31) | ((g&63) << 5 | ((r&31) << 11)))

#define COLOR(r,g,b) ((r<<16) | (g<<8) | b)
#define WHITE COLOR(255,255,255)
#define DARKGRAY COLOR(154,154,154)

static int lfb_px = 0;
static int lfb_py = 0;

static ULONG lfb_width = 0;
static ULONG lfb_height = 0;
static ULONG lfb_depth = 0;
static ULONG lfb_type = 0;

void lfb_clear()
{
	int i;
	int j;

	for (i = 0; i < (int)lfb_width; i++)
		for (j = 0; j < (int)lfb_height; j++)
			lfb[j * lfb_width + i] = 100;
}

ULONG getDepth() {
	return lfb_depth;
}

void init_lfb()
{
	//if (mode_info == nullptr)
		//return;

	void *lfb_ptr;

	lfb_px = 0;
	lfb_py = 0;



	lfb_width = SkyGUISystem::GetInstance()->GetVideoRamInfo()._width;
	lfb_height = SkyGUISystem::GetInstance()->GetVideoRamInfo()._height;
	lfb_depth = SkyGUISystem::GetInstance()->GetVideoRamInfo()._bpp;
	lfb_type = 0;
	lfb_ptr = (void*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;

	lfb = (ULONG*)lfb_ptr;
	
}

void putpixel(ULONG x, ULONG y, ULONG col) {
	lfb[(y*1024) + x] = col;
}

void ppo(ULONG *buffer, ULONG i, unsigned char r, unsigned char g, unsigned char b) {
	buffer[i] = (r << 16) | (g << 8) | b;
}

void pp(ULONG i, unsigned char r, unsigned char g, unsigned char b) {
	lfb[i] = (r << 16) | (g << 8) | b;
}

ULONG getp(ULONG i) {
	return lfb[i];
}

void putp(ULONG i, ULONG col) {
	lfb[i] = col;
}

void lfb_update(unsigned long *buf) {
	unsigned long c;
	unsigned long *p = lfb, *p2 = buf;

	for (c = 0; c<1024 * 768; c++) {
		*p = *p2;
		p++;
		p2++;
	}

}

unsigned long *getlfb() {
	return lfb;
}
