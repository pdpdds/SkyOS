#include "windef.h"
#include "wnd.h"
#include "bar.h"
//#include "VESA.h"

extern "C" unsigned long *getOffscreenBuffer();

unsigned char *bar;
extern void DrawToken(unsigned char *buffer, unsigned long *target, int x2, int y2, int width, int height);

void barUpdate(struct WND *ctrl, struct WND *window, unsigned char uselfb, unsigned fromKbdDriver) {
	DrawToken(bar, getOffscreenBuffer(), ctrl->rect.x, ctrl->rect.y, ctrl->rect.width, ctrl->rect.height); 
}

void init_bar() {
	struct WND *window;
	//struct WND *ctrl = (struct WND*) kalloc(sizeof(struct WND));
	
	//bar = (unsigned char*) kalloc(cdiso_getfilesize("TASKBAR.RAW"));
	//cdiso_readfile("TASKBAR.RAW", bar);
	
	
	//window = NewWindow("", 0, 768-BAR_HEIGHT, 1024, BAR_HEIGHT);
	//window->first_ctrl=ctrl;
	/*
	ctrl->rect.x=0;
	ctrl->rect.y=768-BAR_HEIGHT;
	ctrl->rect.width=1024;
	ctrl->rect.height=BAR_HEIGHT;
	ctrl->needs_repaint=1;
	ctrl->needs_keyboard=0;
	ctrl->control_update_function=barUpdate;
	ctrl->next=NULL;
	ctrl->next_ctrl=NULL;
	ctrl->prev_ctrl=window;	*/
}
