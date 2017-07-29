#pragma once
#include "windef.h"
#include "multiboot.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COLOR(r,g,b) ((r<<16) | (g<<8) | b)
	//#define WHITE COLOR(255,255,255)
	//#define DARKGRAY COLOR(154,154,154)

	void init_lfb(multiboot_info_t *multiboot_info);
	void lfb_clear();
	ULONG getDepth();
	unsigned long *getOffscreenBuffer();
	static ULONG *lfb = 0;
	void ppo(unsigned long *buffer, unsigned long i, unsigned char r, unsigned char g, unsigned char b);
	void lfb_update(unsigned long *buf);
	unsigned long *getlfb();
	extern void putp(ULONG i, ULONG col);

#ifdef __cplusplus
}
#endif