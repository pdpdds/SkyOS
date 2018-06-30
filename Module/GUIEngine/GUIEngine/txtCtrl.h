#ifndef __TXTCTRL_H
#define __TXTCTRL_H

struct textcontrol_t {
	unsigned char *bitmap;
	unsigned char *keyboard_buffer;
	void *command_callback;
	unsigned int scroll;
};

void addTextControl(struct WND *window, int x, int y, int width, int height, void *callback);
struct WND *NewWindow(char *caption, int x, int y, int width, int height);
void wnd_update();

#endif
