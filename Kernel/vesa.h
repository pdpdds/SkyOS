#pragma once
#include "windef.h"
#include "multiboot.h"



typedef struct {
	uint16_t	Offset;
	uint16_t	Segment;
}	t_farptr;

struct VesaControllerInfo {
	char	Signature[4];	// == "VBE2"
	uint16_t	Version;	// == 0x0300 for Vesa 3.0
	t_farptr	OemString;	// isa vbeFarPtr
	uint8_t	Capabilities[4];
	t_farptr	Videomodes;	// isa vbeParPtr
	uint16_t	TotalMemory;// as # of 64KB blocks
};

struct VesaModeInfo {
	uint16_t	attributes;
	uint8_t	winA, winB;
	uint16_t	granularity;
	uint16_t	winsize;
	uint16_t	segmentA, segmentB;
	t_farptr	realFctPtr;
	uint16_t	pitch; // bytes per scanline

	uint16_t	Xres, Yres;
	uint8_t	Wchar, Ychar, planes, bpp, banks;
	uint8_t	memory_model, bank_size, image_pages;
	uint8_t	reserved0;

	uint8_t	red_mask, red_position;
	uint8_t	green_mask, green_position;
	uint8_t	blue_mask, blue_position;
	uint8_t	rsv_mask, rsv_position;
	uint8_t	directcolor_attributes;

	uint32_t	physbase;  // your LFB address ;)
	uint32_t	reserved1;
	uint16_t	reserved2;
};

#ifdef __cplusplus
extern "C" {
#endif

#define COLOR(r,g,b) ((r<<16) | (g<<8) | b)
	//#define WHITE COLOR(255,255,255)
	//#define DARKGRAY COLOR(154,154,154)

	void init_lfb(VesaModeInfo *mode_info);
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