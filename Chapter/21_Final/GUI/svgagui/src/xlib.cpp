/*************************************************************************
	xlib.c -- last change: 18-1-1999

	Copyright (C) 1996-2007  Boris Nagels

	This file is part of SVGAgui.

	SVGAgui is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	SVGAgui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with SVGAgui; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 *************************************************************************/
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "local.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>


static unsigned long color[50];
static int mouse_x = 0, mouse_y = 0, mouse_button = FALSE;
static char char_buf[1] = {'\0'};
static KeySym key;
static XComposeStatus compose;
static int depth = 0;


static unsigned long getXcolor(int unsigned long def)
{
	XColor col;
	unsigned long value;

	col.pixel = def;
	col.red = ((def >> 16) * 256);
	col.green = ((def >> 8) & 0xFF) * 256;
	col.blue = (def & 0xFF) * 256;
	col.flags = DoBlue | DoGreen | DoRed;
	if (!XAllocColor(guiscreen.display, guiscreen.cmap, &col)) {
		fprintf(stderr, "Cannot allocate color #%ldx\n", def);
		value = BlackPixel(guiscreen.display, guiscreen.screen_num);
	} else
		value = col.pixel;
		
	return value;
}        
                                                                                 

static void define_16bit_colors(void)
{
	/* Define the extra colors */
	BLACK      = 1;
	DARKGREY   = 2;
	GREY       = 3;
	LIGHTGREY  = 4;
	WHITE      = 5;
	DARKRED    = 6;
	RED        = 7;
	DARKBLUE   = 8;
	BLUE       = 9;
	LIGHTBLUE  = 10;
	DARKGREEN  = 11;
	GREEN      = 12;
	DARKYELLOW = 13;
	YELLOW     = 14;

	/* redefine the colorscheme */
	BACKGROUND        = 0;
	WIN_BACK          = 15;
	TITLE_BACK        = DARKGREY;
	TITLE_FORE        = GREY;
	ACTIVE_TITLE_BACK = DARKBLUE;
	ACTIVE_TITLE_FORE = WHITE;
	BUTTON_BACK       = GREY;
	BUTTON_FORE       = BLACK;
	SLIDER_BACK       = LIGHTGREY;
	SLIDER_FORE       = LIGHTGREY;
	TEXT_EMBOSS       = DARKGREY;
	TEXT_NORMAL       = BLACK;
	INPUT_BACK        = GREY;
	INPUT_FORE        = BLACK;
	ACTIVE_INPUT_BACK = LIGHTGREY;
	BROWSER_BACK      = WHITE;
	BROWSER_FORE      = BLACK;
	INFO_BACK         = 16;
	INFO_FORE         = BLACK;
	CHOICE_BACK       = WHITE;
	CHOICE_FORE       = BLACK;
	LISTBOX_BACK      = WHITE;
	LISTBOX_FORE      = BLACK;
	NUMBER_FORE       = 17;

	color[BACKGROUND]  = getXcolor(0x000000);
	color[WIN_BACK]    = getXcolor(0xC0C0C0);
	color[INFO_BACK]   = getXcolor(0xDCDCA8);
	color[NUMBER_FORE] = getXcolor(0x4880B8);

	color[BLACK]      = getXcolor(0x000000);
	color[DARKGREY]   = getXcolor(0x909090);
	color[GREY]       = getXcolor(0xC0C0C0);
	color[LIGHTGREY]  = getXcolor(0xD8D8D8);
	color[WHITE]      = getXcolor(0xFFFFFF);
	color[DARKRED]    = getXcolor(0x780000);
	color[RED]        = getXcolor(0xE80000);
	color[DARKBLUE]   = getXcolor(0x000080);
	color[BLUE]       = getXcolor(0x0000C0);
	color[LIGHTBLUE]  = getXcolor(0x0080F0);
	color[DARKGREEN]  = getXcolor(0x007800);
	color[GREEN]      = getXcolor(0x54FF54);
	color[DARKYELLOW] = getXcolor(0xFFBC00);
	color[YELLOW]     = getXcolor(0xFFFF54);
}


static void define_8bit_colors(void)
{
	/* Define the extra colors */
	BACKGROUND  = getXcolor(0x000000);
	WIN_BACK    = getXcolor(0xC0C0C0);
	INFO_BACK   = getXcolor(0xDCDCA8);
	NUMBER_FORE = getXcolor(0x4880B8);

	BLACK      = getXcolor(0x000000);
	DARKGREY   = getXcolor(0x909090);
	GREY       = getXcolor(0xC0C0C0);
	LIGHTGREY  = getXcolor(0xD8D8D8);
	WHITE      = getXcolor(0xFFFFFF);
	DARKRED    = getXcolor(0x780000);
	RED        = getXcolor(0xE80000);
	DARKBLUE   = getXcolor(0x000080);
	BLUE       = getXcolor(0x0000C0);
	LIGHTBLUE  = getXcolor(0x0080F0);
	DARKGREEN  = getXcolor(0x007800);
	GREEN      = getXcolor(0x54FF54);
	DARKYELLOW = getXcolor(0xFFBC00);
	YELLOW     = getXcolor(0xFFFF54);

	/* redefine the colorscheme */
	TITLE_BACK        = DARKGREY;
	TITLE_FORE        = GREY;
	ACTIVE_TITLE_BACK = DARKBLUE;
	ACTIVE_TITLE_FORE = WHITE;
	BUTTON_BACK       = GREY;
	BUTTON_FORE       = BLACK;
	SLIDER_BACK       = LIGHTGREY;
	SLIDER_FORE       = LIGHTGREY;
	TEXT_EMBOSS       = DARKGREY;
	TEXT_NORMAL       = BLACK;
	INPUT_BACK        = GREY;
	INPUT_FORE        = BLACK;
	ACTIVE_INPUT_BACK = LIGHTGREY;
	BROWSER_BACK      = WHITE;
	BROWSER_FORE      = BLACK;
	INFO_FORE         = BLACK;
	CHOICE_BACK       = WHITE;
	CHOICE_FORE       = BLACK;
	LISTBOX_BACK      = WHITE;
	LISTBOX_FORE      = BLACK;
}


void open_X_screen(int width, int height, int colors, char *title)
{
	XSetWindowAttributes attr;
	XGCValues gcv;
	XSizeHints *xsh;
	XEvent xev;
	unsigned long *src;
	char *data = NULL;
	unsigned long black, white;
	int i;

	/* restore the default user- and group ID */
	setuid(getuid());
	setgid(getgid());

	guiscreen.display = XOpenDisplay(NULL);
	guiscreen.screen_num = DefaultScreen(guiscreen.display);
	guiscreen.cmap = DefaultColormap(guiscreen.display, guiscreen.screen_num);
	depth = DefaultDepth(guiscreen.display, guiscreen.screen_num);

	black = BlackPixel(guiscreen.display, guiscreen.screen_num);
	white = WhitePixel(guiscreen.display, guiscreen.screen_num);

	guiscreen.event_mask = ButtonPressMask | ButtonReleaseMask |
			       ExposureMask | PointerMotionMask | KeyPressMask;
	attr.event_mask = guiscreen.event_mask;
	attr.background_pixel = black;

	guiscreen.win = XCreateSimpleWindow(guiscreen.display, DefaultRootWindow(guiscreen.display),
				   0, 0, width, height, 0, white, black);
	XSelectInput(guiscreen.display, guiscreen.win, guiscreen.event_mask);

	/* Change Window and icon Title */
	XChangeProperty(guiscreen.display, guiscreen.win, XA_WM_NAME, XA_STRING,
			8, PropModeReplace, (unsigned char *) title,
			strlen(title) );
	XChangeProperty(guiscreen.display, guiscreen.win, XA_WM_ICON_NAME, XA_STRING,
			8, PropModeReplace, (unsigned char *) title,
			strlen(title) );

	/* set min and max geometry */
	if (NULL == (xsh = XAllocSizeHints())) {
		error("XAllocSizeHints() failed.");
	}
	xsh->flags = PPosition | PSize | PMinSize | PMaxSize;
	xsh->min_width = width;
	xsh->max_width = width;
	xsh->min_height = height;
	xsh->max_height = height;
	XSetWMSizeHints(guiscreen.display, guiscreen.win, xsh, XA_WM_NORMAL_HINTS);

	/* Map the X-window */
	XMapRaised(guiscreen.display, guiscreen.win);

	/* Wait for the X-window to be drawn by the server */
	do {
		XNextEvent(guiscreen.display, &xev);
	}
	while (xev.type != Expose);

	fprintf(stderr, "Starting on X-display (%dx%d, depth = %d bit)...\n", width, height, depth);

	/* Set up a graphics context: */
	gcv.foreground = black;
	gcv.function = GXcopy;
	guiscreen.gc = XCreateGC(guiscreen.display, guiscreen.win, 
				GCForeground | GCFunction, &gcv);
	if (depth == 8)
		define_8bit_colors();
	else {
		define_16bit_colors();
		if (guiscreen.image_data != NULL)
			free(guiscreen.image_data);
		guiscreen.image_data = (unsigned long *)malloc(width * height * sizeof(unsigned long));
		if (guiscreen.image_data == NULL)
			error("Cannot allocate data array in open_X_screen().");
		data = (char *)guiscreen.image_data;
		/* initialize the color of the image to black */
		src = guiscreen.image_data;
		for (i = 0;i < width * height;i++)
			*(src++) = color[BLACK];
	}

	/* Allocate memory for the screen image */
	if (guiscreen.data != NULL)
		free(guiscreen.data);
	guiscreen.data = (char *) malloc(width * height);
	if (guiscreen.data == NULL)
		error("Cannot allocate memory for screen operations in open_screen().");
	if (depth == 8)
		data = guiscreen.data;
	memset(guiscreen.data, (char)BLACK, width * height);
	
	guiscreen.image = XCreateImage(
		guiscreen.display, DefaultVisual(guiscreen.display, guiscreen.screen_num),
		depth, ZPixmap, 0, data, width, height, 8, 0);

	XInitImage(guiscreen.image);

	XPutImage(guiscreen.display, guiscreen.win, guiscreen.gc, 
		  guiscreen.image, 0, 0, 0, 0, width, height);

	guiscreen.type = XLIB;
}


void close_X_screen(void)
{
	if (guiscreen.type == XLIB) {
		XDestroyImage(guiscreen.image);
		XFreeGC(guiscreen.display, guiscreen.gc);
		XDestroyWindow(guiscreen.display, guiscreen.win);
		XCloseDisplay(guiscreen.display);
	}
}


void update_X_screen(void)
{
	int length, line, x, image_depth;
	char *src, *dst8;
	short int *dst16;
	unsigned long *dst32;
	
	image_depth = guiscreen.image->bits_per_pixel;
	if (image_depth == 32) {
		src = guiscreen.data + guiscreen.x_min + guiscreen.y_min * guiscreen.width;
		length = guiscreen.x_max - guiscreen.x_min + 1;
		dst32 = guiscreen.image_data + guiscreen.x_min + guiscreen.y_min * guiscreen.width;
		for (line = 0; line < guiscreen.y_max - guiscreen.y_min + 1;line++) {
			for (x = 0;x < length;x++)
				*(dst32++) = *(color + *src++);
			dst32 += guiscreen.width - length;
			src += guiscreen.width - length;
		}
	} else if (image_depth == 24) {
		src = guiscreen.data + guiscreen.x_min + guiscreen.y_min * guiscreen.width;
		length = guiscreen.x_max - guiscreen.x_min + 1;
		dst8 = (char *)guiscreen.image_data + (guiscreen.x_min + guiscreen.y_min * guiscreen.width) * 3;
		for (line = 0; line < guiscreen.y_max - guiscreen.y_min + 1;line++) {
			for (x = 0;x < length;x++) {
				*((unsigned long *)dst8) = *(color + *src++);
				dst8 += 3;
			}
			dst8 += (guiscreen.width - length) * 3;
			src += guiscreen.width - length;
		}
	} else if (image_depth > 8) {
		src = guiscreen.data + guiscreen.x_min + guiscreen.y_min * guiscreen.width;
		length = guiscreen.x_max - guiscreen.x_min + 1;
		dst16 = (short int *)guiscreen.image_data + guiscreen.x_min + guiscreen.y_min * guiscreen.width;
		for (line = 0; line < guiscreen.y_max - guiscreen.y_min + 1;line++) {
			for (x = 0;x < length;x++)
				*(dst16++) = *(color + *src++);
			dst16 += guiscreen.width - length;
			src += guiscreen.width - length;
		}
	}

	XPutImage(guiscreen.display, guiscreen.win, guiscreen.gc,
		  guiscreen.image, guiscreen.x_min, guiscreen.y_min,
		  guiscreen.x_min, guiscreen.y_min, guiscreen.x_max -
		  guiscreen.x_min + 1, guiscreen.y_max - guiscreen.y_min + 1);
}


void init_X_mouse(void)
{
}


void get_X_mouse_position(void)
{
	mouse.x = mouse_x;
	mouse.y = mouse_y;
}


void set_X_mouse_position(int x, int y)
{
	XWarpPointer(guiscreen.display, guiscreen.win, guiscreen.win,
		     0, 0, 0, 0, x, y);
}


void set_X_default_palette(int colors)
{
}


void set_X_palette(int color, int red, int green, int blue)
{
}


void get_X_palette(int color, int *red, int *green, int *blue)
{
	*red = 0;
	*green = 0;
	*blue = 0;
}


static void check_mouse_range(int *pos, int min, int max)
{
	if (*pos < min)
		*pos = min;
	else if (*pos > max)
		*pos = max;
}


int get_X_message(void)
{
	int message = FALSE;
	long move_mask = PointerMotionMask;

	if (XCheckMaskEvent(guiscreen.display, guiscreen.event_mask, &guiscreen.event))
	switch (guiscreen.event.type) {
	case Expose:
		XPutImage(guiscreen.display, guiscreen.win, guiscreen.gc,
			  guiscreen.image, 0, 0, 0, 0, guiscreen.width,
			  guiscreen.height);
		break;
	case MotionNotify:
		/* Clear all pending pointer movements */
		while (XCheckMaskEvent(guiscreen.display, move_mask, &guiscreen.event)) ;

		mouse_x = guiscreen.event.xmotion.x;
		check_mouse_range(&mouse_x, 0, guiscreen.width);
		mouse_y = guiscreen.event.xmotion.y;
		check_mouse_range(&mouse_y, 0, guiscreen.height);
		message = GuiMouseEvent;
		break;
	case ButtonPress:
		switch(guiscreen.event.xbutton.button) {
			case 1:
				mouse_button = GuiMouseLeftButton;
				break;
			case 2:
				mouse_button = GuiMouseMiddleButton;
				break;
			case 3:
				mouse_button = GuiMouseRightButton;
				break;
		}
		message = GuiMouseEvent;
		break;
	case ButtonRelease:
		mouse_button = FALSE;
		message = GuiMouseEvent;
		break;
	case KeyPress:
		XLookupString((XKeyEvent *)&guiscreen.event, char_buf, 1, &key, &compose);
		message = GuiKeyboardEvent;
		break;
	}
	
	return message;
}


int get_X_mouse_button(void)
{
	return mouse_button;
}


char get_X_keyboard_char(void)
{
	return char_buf[0];
}


void create_X_cursor(int id)
{
	XColor white, black, dummyColor;
	Pixmap curPix, maskPix;
	Colormap colormap;

	colormap = DefaultColormap(guiscreen.display, guiscreen.screen_num);
	
	curPix = XCreateBitmapFromData(guiscreen.display, guiscreen.win, 
			cursor[id].cur_bits, cursor[id].cur_width, cursor[id].cur_height);
	maskPix = XCreateBitmapFromData(guiscreen.display, guiscreen.win,
			cursor[id].mask_bits, cursor[id].mask_width, cursor[id].mask_height);

	XAllocNamedColor(guiscreen.display, colormap, "black", &black, &dummyColor);
	XAllocNamedColor(guiscreen.display, colormap, "white", &white, &dummyColor);

	cursor[id].cursor = XCreatePixmapCursor(guiscreen.display, curPix, maskPix, 
						&black, &white, cursor[id].x_hot, cursor[id].y_hot);
}


void switch_X_cursor(int id)
{
	XDefineCursor(guiscreen.display, guiscreen.win, cursor[id].cursor);
}
#endif