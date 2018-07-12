#include "GUIEngine.h"
#include "windef.h"
#include "string.h"
#include "memory.h"
#include "wnd.h"
#include "txtCtrl.h"
#include "svga_window.h"
#include "VESA.h"
#include "WindowManagerTask.h"
#include "Queue.h"



extern shell Shell;
extern void gui_init(unsigned char *wallpaper_ptr);
extern "C" void SHLaunchShell();
extern "C" void SHinitShell(char *message, char *title, int width, int height);

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
	//ULONG* lfAb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;
	//SampleFillRect(lfAb, 0, 0, 20, 20, 0x00ff0000);
	window1 = NewWindow("LikePad Terminal", 300, 200, 640, 480);
	//SampleFillRect(lfAb, 20, 20, 20, 20, 0x00ff0000);
	addTextControl(window1, 0, 0, 640, 480, LikePad_Command_Callback);

	window2 = NewWindow("LikePad Terminal", 100, 100, 640, 480);
	addTextControl(window2, 0, 0, 640, 480, LikePad_Command_Callback);

	wnd_update();
}





extern "C"
void SHLaunchExplorer() {
	window *w = new window();
	menu *me = new menu();
	combobox *c = new combobox();
	label *l = new label();
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

GUIEngine::GUIEngine()
{
	
}


GUIEngine::~GUIEngine()
{
}
extern guiroot root;
bool GUIEngine::Initialize() 
{
	extern void kStartWindowManager(void);
	kStartWindowManager();

	//gui_init(0);
	//Launch_LikePad();

	
	//root.init();
	//SHLaunchExplorer();
	//SHinitShell("SkyOS", "Explorer", 320, 240);
	//SHLaunchShell();

	return true;
}
void GUIEngine::Update(float deltaTime) 
{
	
	kUpdate(m_linearBufferInfo.isDirectVideoBuffer);
}

void GUIEngine::SetLinearBuffer(LinearBufferInfo& linearBufferInfo)
{
	m_linearBufferInfo = linearBufferInfo;
	init_lfb(linearBufferInfo.pBuffer, linearBufferInfo.width, linearBufferInfo.height, linearBufferInfo.depth, linearBufferInfo.type);
}

extern QUEUE gs_stKeyQueue;
extern QUEUE gs_stMouseQueue;
bool GUIEngine::PutKeyboardQueue(KEYDATA* pData)
{
	return kPutQueue(&gs_stKeyQueue, pData);
}

bool  GUIEngine::PutMouseQueue(MOUSEDATA* pData)
{
	return kPutQueue(&gs_stMouseQueue, pData);
}


//int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF };
//ULONG* lfAb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;
//SampleFillRect(lfAb, 0, 0, 20, 20, 0x00ff0000);