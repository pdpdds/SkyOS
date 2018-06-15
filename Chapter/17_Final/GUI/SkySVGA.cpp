#include "SkySVGA.h"
#include "svgaguientry.h"
#include "VESA.h"
#include "Hal.h"
#include "svga_mouse.h"
#include "svga_keyb.h"
#include "SkyAPI.h"
#include "SkyOS.h"
#include "SkyGUISystem.h"

extern void kSVGAKeyboardHandler();
extern void kSVGAMouseHandler();

SkySVGA::SkySVGA()
{
}


SkySVGA::~SkySVGA()
{
}
#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1

#define PIC1_ICW4		0x00a1

extern void wait_KBC_sendready(void);
extern void mouse_init();
bool SkySVGA::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
{	
	init_lfb();
	//lfb_clear();

	

	kEnterCriticalSection();
	SetInterruptVector(0x21, kSVGAKeyboardHandler);
	SetInterruptVector(0x2c, kSVGAMouseHandler);
	kLeaveCriticalSection();

	mouse_init();
	return true;
}

bool SkySVGA::Run()
{	
	

	StartSampleGui();
	
	for (;;);

	return false;
}

bool SkySVGA::Print(char* pMsg)
{
	return false;
}

bool SkySVGA::Clear()
{
	return false;
}