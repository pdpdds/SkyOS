#ifndef __VGA_H
#define __VGA_H

#include "windef.h"

#define VGA_MISC_REG        0x03C2
#define VGA_ENABLE_REG      0x03C3
#define VGA_STATUS_REG 		0x03DA
#define VGA_FEATURE_REG 	0x03DA

//Sequencer 
#define VGA_SEQUENCER_ADDRESS 0x3C4
#define VGA_SEQUENCER_DATA    0x3C5

//CRT
#define VGA_CRT_ADDRESS 0x3D4
#define VGA_CRT_DATA    0x3D5
//Graphics
#define VGA_GRAPHICS_ADDRESS 0x3CE
#define VGA_GRAPHICS_DATA    0x3CF
#define VGA_GRAPHICS_BIT_MASK   0x8

//Attribute
#define VGA_ATTRIB_ADDRESS       0x3C0
#define VGA_ATTRIB_DATA_WRITE    0x3C0
#define VGA_ATTRIB_DATA_READ     0x3C1

// DAC Registers 
#define VGA_DAC_PEL_MASK       0x3C6
#define VGA_DAC_READ_ADDRESS   0x3C7
#define VGA_DAC_WRITE_ADDRESS  0x3C8
#define VGA_DAC_DATA 	       0x3C9

#define VGA_MODE_640_480 	12
#ifdef __cplusplus
	extern "C" {
#endif
UINT32 VGAGetPixel(UINT16 X, UINT16 Y);
void VGAGetByte(UINT32 Offset, BYTE * Blue, BYTE * Green, BYTE * Red, BYTE * Intensity);
void VGAPutByte(UINT16 X, UINT16 Y, BYTE Value);
void VGAPutPixel(UINT16 X, UINT16 Y, BYTE Color);
void SetRGBPalette16();
void SetRGBPalette(BYTE Color, BYTE Red, BYTE Green, BYTE Blue);
BYTE VGASetMode(BYTE Mode);
BYTE VGAGetMode();
void VGAPreCalc();
void InitVGA();
#ifdef __cplusplus
	}
#endif

#endif
