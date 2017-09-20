/* Created by Sam on 5-Feb-2002	samuelhard@yahoo.com
	modified on 12-Dec-2002 to work in Ace OS

Credits
	1) IBM's document vga_app_mak.html from rs6000.com but it is removed from there now
		It gives me a general idea and vga registers address. wonderfull
	2) PROGRAMMER'S GUIDE TO PC(R) AND PS/2(TM) VIDEO SYSTEMS by RICHARD WILTON 
	3) Tauron VGA Utilities Version 3.0 by Jeff Morgan
		E-mail:      kinfira@hotmail.com
		Homepage:    http://home.onestop.net/kinfira/
	4) React OS - Open Source NT 	
		I copied many functions from there
*/
#include "windef.h"
#include <StdVGA.h>
#include <StdIO.h>
#include "Hal.h"
#include "SkyConsole.h"
#include "kheap.h"

BYTE __VGAMode=0;
BYTE __VGAMode640_480[62] = 
	{
	// MISC reg,  STATUS reg,    SEQ regs
	0xE3,    0x00,     0x03,0x01,0x0F,0x00,0x06,
	// CRTC regs
	0x5F,0x4F,0x50,0x82,0x54,0x80,0x0B,0x3E,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x59,
	0xEA,0x8C,0xDF,0x28,0x00,0xE7,0x04,0xE3,0xFF,
	// GRAPHICS regs
	0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0F,0xFF,
	// ATTRIBUTE CONTROLLER regs
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x81,0x00,0x0F,0x00,0x00
	};
char __VGA_RGBPalette[48] = 
	{
		  0,  0,  0,	//black
		  0,  0, 42, 	//dark blue
		  0, 42,  0, 	//dark green
		  0, 42, 52, 	//pale blue
		 42,  0,  0, 	//crimson red
		 42, 63, 12,	
		168,173, 44,
		42, 42, 42, 
		21, 21, 21,
		21, 21, 63, 
		21, 63, 21,
		21, 63, 63,
		63, 21, 21, 
		63, 21, 63, 
		63, 63, 21, 
		63, 63, 63,  
	};
UINT32 StartMasks[9]={0, 127,  63,  31,  15,  7,   3,    1, 255};
UINT32 EndMasks[9]={128, 192, 224, 240, 248, 252, 254, 255, 255};
BYTE * VGAVideoMemory;
UINT32 * MaskBit, *Y80, *XConv, *Bit8;
void VGAPreCalc()
	{MaskBit=(UINT32 *)kmalloc(640*4);
	Y80=(UINT32 *)kmalloc(640*4);
	XConv=(UINT32 *)kmalloc(640*4);
	Bit8=(UINT32 *)kmalloc(640*4);

	UINT32 j;
	for(j=0; j<80; j++)
		{MaskBit[j*8]   = 128;
		MaskBit[j*8+1] = 64;
		MaskBit[j*8+2] = 32;
		MaskBit[j*8+3] = 16;
		MaskBit[j*8+4] = 8;
		MaskBit[j*8+5] = 4;
		MaskBit[j*8+6] = 2;
		MaskBit[j*8+7] = 1;

		Bit8[j*8]   = 7;
		Bit8[j*8+1] = 6;
		Bit8[j*8+2] = 5;
		Bit8[j*8+3] = 4;
		Bit8[j*8+4] = 3;
		Bit8[j*8+5] = 2;
		Bit8[j*8+6] = 1;
		Bit8[j*8+7] = 0;
		}
	for(j=0; j<480; j++)
		Y80[j]  = j*80;
	for(j=0; j<640; j++)
		XConv[j] = j >> 3;
	}
BYTE VGAGetMode()
	{return __VGAMode;
	}
BYTE VGASetMode(BYTE Mode)
	{BYTE * ModeParameters = __VGAMode640_480,i;

if (Mode != VGA_MODE_640_480)
{
	SkyConsole::Print("\n\r VGA :: Invalid mode - %d.640x480x16 is the only mode supported", Mode);
	return 0;
}
	__asm cli
	OutPortByte(VGA_MISC_REG, *ModeParameters);
	ModeParameters++;

	OutPortByte(VGA_STATUS_REG, 0);

	OutPortByte(VGA_FEATURE_REG, *ModeParameters);
	ModeParameters++;
	
	for(i=0;i<5;i++)
		{OutPortByte(VGA_SEQUENCER_ADDRESS , i);
		OutPortByte(VGA_SEQUENCER_DATA , *ModeParameters);
		*ModeParameters++;
		}
	OutPortByte(VGA_CRT_ADDRESS, 0x11);
	OutPortByte(VGA_CRT_DATA, 0x0E & 0x7F );
	
	for(i=0;i<25;i++)
		{OutPortByte(VGA_CRT_ADDRESS , i);
		OutPortByte(VGA_CRT_DATA , *ModeParameters);
		*ModeParameters++;
		}

	for(i=0;i<9;i++)
		{OutPortByte(VGA_GRAPHICS_ADDRESS , i);
		OutPortByte(VGA_GRAPHICS_DATA , *ModeParameters);
		*ModeParameters++;
		}

	InPortByte(VGA_FEATURE_REG);
	for(i=0;i<21;i++)
		{//InPortWord(VGA_ATTRIB_ADDRESS);
		OutPortByte(VGA_ATTRIB_ADDRESS,i);
		OutPortByte(VGA_ATTRIB_DATA_WRITE , *ModeParameters);
		*ModeParameters++;
		}
	OutPortByte(VGA_ATTRIB_DATA_WRITE , 0x20);
	
	__VGAMode=VGA_MODE_640_480;
	__asm sti
	return __VGAMode;
	}
void SetRGBPalette(BYTE Color, BYTE Red, BYTE Green, BYTE Blue)
	{
	OutPortByte(VGA_DAC_WRITE_ADDRESS, Color);
	OutPortByte(VGA_DAC_DATA, Red);
	OutPortByte(VGA_DAC_DATA, Green);
	OutPortByte(VGA_DAC_DATA, Blue);
	}
void SetRGBPalette16()
	{
	BYTE ColorNum = 0,i;
	__asm cli
	for (i = 0; i < 48; i+=3)
		{SetRGBPalette(ColorNum, __VGA_RGBPalette[i], __VGA_RGBPalette[i+1], __VGA_RGBPalette[i+2]);
		ColorNum++;
		}
	__asm sti;
	}
void VGAPutPixel(UINT16 X, UINT16 Y, BYTE Color)
	{
	UINT32 Offset;

	Offset = XConv[X]+Y80[Y];

	OutPortByte( VGA_GRAPHICS_ADDRESS, VGA_GRAPHICS_BIT_MASK );
	OutPortByte( VGA_GRAPHICS_DATA, MaskBit[X] );

	BYTE tmp=*(VGAVideoMemory + Offset);
	*(VGAVideoMemory + Offset) = Color;
	}
void VGAPutByte(UINT16 X, UINT16 Y, BYTE Value)
	{
	UINT32 Offset;

	Offset = XConv[X]+Y80[Y];

	OutPortByte( VGA_GRAPHICS_ADDRESS, VGA_GRAPHICS_BIT_MASK );
	OutPortByte( VGA_GRAPHICS_DATA, 0xFF );

	//BYTE tmp=*(VGAVideoMemory + Offset);
	*(VGAVideoMemory + Offset) = Value;
	}
void VGAGetByte(UINT32 Offset, BYTE * Blue, BYTE * Green, BYTE * Red, BYTE * Intensity)
	{
	OutPortWord(VGA_GRAPHICS_ADDRESS, 0x0304);
  	*Intensity = *(VGAVideoMemory + Offset);
	OutPortWord(VGA_GRAPHICS_ADDRESS, 0x0204);
	*Red =  *(VGAVideoMemory + Offset);
	OutPortWord(VGA_GRAPHICS_ADDRESS, 0x0104);
	*Green =  *(VGAVideoMemory + Offset);
	OutPortWord(VGA_GRAPHICS_ADDRESS, 0x0004);
	*Blue =  *(VGAVideoMemory + Offset);
	}

UINT32 VGAGetPixel(UINT16 X, UINT16 Y)
	{BYTE Mask, Blue, Green, Red, Intensity;
	UINT32 Offset;

	Offset = XConv[X]+Y80[Y];
	VGAGetByte(Offset, &Blue, &Green, &Red, &Intensity);

	Mask=MaskBit[X];
	Blue&=Mask;
	Green&=Mask;
	Red&=Mask;
	Intensity&=Mask;

	Mask=Bit8[X];
	Green>>=Mask;
	Blue>>=Mask;
	Red>>=Mask;
	Intensity>>=Mask;

	return(Blue+2*Green+4*Red+8*Intensity);
	}
void FillScreen(BYTE Color)
	{UINT32 i;

	OutPortByte(VGA_GRAPHICS_ADDRESS, VGA_GRAPHICS_BIT_MASK);
	OutPortByte(VGA_GRAPHICS_DATA, 0xff);
	for(i=0;i<64000;i++)
		*(VGAVideoMemory+i)=Color;
	}
void InitVGA()	
{
	UINT16 i,j;
	VGAVideoMemory=(BYTE *)0x400000;
	VGAPreCalc();
	VGASetMode(VGA_MODE_640_480);
	SetRGBPalette16();
	OutPortByte(VGA_GRAPHICS_ADDRESS, VGA_GRAPHICS_BIT_MASK);
	OutPortByte(VGA_GRAPHICS_DATA, 0);
	OutPortWord(VGA_GRAPHICS_ADDRESS, 0x0205); // write mode = 2 (bits 0,1) read mode = 0  (bit 3)
	i = *VGAVideoMemory; // Update bit buffer
	*VGAVideoMemory=0;	 // Write the pixel

	FillScreen(0x1);				//Filling Blue screen
	for(i=0;i<640;i+=2)
		for(j=0;j<480;j++)
			VGAPutPixel(i,j,0);		//Texturing
	}

