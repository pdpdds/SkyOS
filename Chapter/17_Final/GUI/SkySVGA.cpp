#include "SkySVGA.h"
#include "svgaguientry.h"
#include "VESA.h"
#include "Hal.h"
#include "svga_mouse.h"
#include "svga_keyb.h"
#include "SkyAPI.h"

#include "wnd.h"
#include "txtCtrl.h"
#include "svga_window.h"

#include "SkyOS.h"
#include "SkyGUISystem.h"

extern void SampleFillRect(ULONG* lfb, int x, int y, int w, int h, int col);

void LikePad_Command_Callback(struct textcontrol_t *txtctrl, char *command) {
	if (strcmp(command, "uname") == 0) {
		strcat((char*)txtctrl->keyboard_buffer, "Like OS v1.0 - Copyright (C) Nikolaos Rangos\r");
	}
	else {
		strcat((char*)txtctrl->keyboard_buffer, "Unknown command\r");
	}
}

void Launch_LikePad() {
	struct WND *window1, *window2, *window3;
	ULONG* lfAb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;
	SampleFillRect(lfAb, 0, 0, 20, 20, 0x00ff0000);
	window1 = NewWindow("LikePad Terminal", 300, 200, 640, 480);
	SampleFillRect(lfAb, 20, 20, 20, 20, 0x00ff0000);
	addTextControl(window1, 0, 0, 640, 480, LikePad_Command_Callback);

	window2 = NewWindow("LikePad Terminal", 100, 100, 640, 480);
	addTextControl(window2, 0, 0, 640, 480, LikePad_Command_Callback);

	wnd_update();
}

extern shell Shell;

extern "C"
void SHLaunchExplorer() {
	window *w = new window();
	menu *me = new menu();
	combobox *c = new combobox();
	label *l = new label();
	//struct __cdiso_directory dir[1024];
	int i, cnt, width = 500, height = 555;

	w->createwindow("LikeOS Explorer", width, height);
	w->invisible = 0;
	w->controlscount = 0;
	w->handle = 1;
	w->z = 1;

	listbox *li = new listbox();
	li->createlistbox("TEST", 5, 46 + 23, width - 12, height - 80);
	li->handle = 1;
	li->windowHandle = 1;
	li->refresh = 1;
	w->controls[w->controlscount] = (control*)li;
	w->controlscount++;

	l->createlabel("Address", 5, 50);
	l->handle = 0;
	l->windowHandle = 1;
	l->refresh = 1;
	w->controls[w->controlscount] = (control*)l;
	w->controlscount++;

	c->createcombobox("CDROM/", 70, 46, width - (32 + 70), 21);
	c->handle = 2;
	c->windowHandle = 1;
	c->refresh = 1;
	c->additem("PARTITION1/");
	c->additem("PARTITION2/");
	c->additem("PARTITION3/");
	c->additem("REMOVEABLE_DISK1/");
	w->controls[w->controlscount] = (control*)c;
	w->controlscount++;

	me->createmenu("TEST", 5, 22, width, 22);
	me->handle = 3;
	me->windowHandle = 1;
	me->refresh = 1;
	me->addtopmenu();
	me->additem("File", 0, 0);
	me->additem("Close", 0, 1);
	me->addtopmenu();
	me->additem("Edit", 1, 0);
	me->additem("Select", 1, 1);
	me->addtopmenu();
	me->additem("View", 2, 0);
	me->additem("Details", 2, 1);
	w->controls[w->controlscount] = (control*)me;
	w->controlscount++;

	//cnt = cdiso_getdirectory(dir);
	//for (i = 0; i<cnt; i++) {
	//	li->addItem((char*)dir[i].Identifier);
	//}

	Shell.windows[Shell.windowscount] = w;
	Shell.windowscount++;
//
	Shell.windowWithFocus = 2;
}

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
extern void gui_init(unsigned char *wallpaper_ptr);
extern "C" void SHLaunchShell();
extern "C" void SHinitShell(char *message, char *title, int width, int height);
bool SkySVGA::Run()
{	
	

	StartSampleGui();
	//gui_init(0);
	//Launch_LikePad();
	
	SHLaunchExplorer();
	SHinitShell("SkyOS", "Explorer", 320, 240);
	int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF };
	ULONG* lfAb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;
	SampleFillRect(lfAb, 0, 0, 20, 20, 0x00ff0000);
	
	SHLaunchShell();
	for (;;);
	
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