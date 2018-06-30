#ifndef __WND_H
#define __WND_H

#define WNDTOPX 6
#define WNDTOPY 21
#define WNDBTMX 7
#define WNDBTMY 4
#define WNDLEFTX 4
#define WNDLEFTY 4
#define WNDRIGHTX 5
#define WNDRIGHTY 6
#define WNDLTX 16
#define WNDLTY 2
#define WNDRTX 13
#define WNDRTY 2
#define WNDLBX 9
#define WNDLBY 2
#define WNDRBX 10
#define WNDRBY 2
#define WNDCTLX 52
#define WNDCTLY 16
#define ITERMX 19
#define ITERMY 17
#define TRANSPARENCY 0.6
#define CONTROL_TYPE_TEXTCONTROL 0
#define MAX_CLICKHANDLERS 10

struct tagRECT {
	int x;
	int y;
	unsigned int width;
	unsigned int height;
};

struct clickhandler {
	void *handler;
	unsigned int x,y,width,height;
};

 struct WND
{
	struct tagRECT rect;	
    unsigned long handle;
    unsigned char *caption;
    unsigned long flags;
    unsigned char needs_repaint;
    unsigned int is_topmost_window;
	void *control_struct;
	int control_type;
	void *control_update_function;
	unsigned char active_window;
	unsigned char needs_keyboard;
	struct clickhandler clickhandlers[MAX_CLICKHANDLERS];
	
    struct WND *prev, *next;
    struct WND *prev_ctrl, *next_ctrl, *first_ctrl;
};

extern WND *wnd_head;
#endif
