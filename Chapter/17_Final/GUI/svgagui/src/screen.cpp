/*************************************************************************
	screen.c -- last change: 20-1-1998

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

#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "local.h"
#include "windef.h"
#include "PIT.h"

GuiScreen guiscreen;
int _clip, _clipx1, _clipx2, _clipy1, _clipy2;

/* define the colorscheme */
unsigned long	BACKGROUND, WIN_BACK,
	TITLE_BACK, TITLE_FORE,
	ACTIVE_TITLE_FORE, ACTIVE_TITLE_BACK,
	BUTTON_BACK, BUTTON_FORE,
	SLIDER_BACK, SLIDER_FORE,
	TEXT_EMBOSS, TEXT_NORMAL, TEXT_HIGHLIGHT, 
	INPUT_BACK, INPUT_FORE, ACTIVE_INPUT_BACK,
	BROWSER_BACK, BROWSER_FORE,
	INFO_BACK, INFO_FORE,
	LISTBOX_BACK, LISTBOX_FORE,
	CHOICE_BACK, CHOICE_FORE,
	NUMBER_FORE;
	
/* Define the extra colors */
unsigned long	BLACK, DARKGREY, GREY, LIGHTGREY, WHITE,
	DARKRED, RED, DARKBLUE, BLUE, LIGHTBLUE,
	DARKGREEN, GREEN, DARKYELLOW, YELLOW;
	

void open_screen(int type, int width, int height, int colors, char *title)
{
	if (!svgagui_initialized)
		error("You must first initialize the library using: init_svgagui().");
	if (type != SVGALIB && type != XLIB)
		error("Not a valid screen type! Use SVGALIB or XLIB.");
		
	guiscreen.type = type;
	switch (type) {
		case SVGALIB:
			open_svga_screen(width, height, colors);
			break;
#if 0
		case XLIB:
			open_X_screen(width, height, colors, title);
			break;
#endif
	}

	guiscreen.width = width;
	guiscreen.height = height;
	guiscreen.colors = colors;

	enable_clipping();
	set_clipping_window(0, 0, guiscreen.width - 1, guiscreen.height - 1);
}


void enable_clipping(void)
{
	_clip = TRUE;
}


void disable_clipping(void)
{
	_clip = FALSE;
}


void set_clipping_window(int x1, int y1, int x2, int y2)
{
	if (!_clip) {
		//fprintf(stderr, "-----> Warning: clipping is not enabled!\n");
		return;
	}
	_clipx1 = x1;
	_clipy1 = y1;
	_clipx2 = x2;
	_clipy2 = y2;
}


void update_screen(void)
{
	if ((guiscreen.x_min == -1) || (guiscreen.x_max == -1))
		return;
	if ((guiscreen.y_min == -1) || (guiscreen.y_max == -1))
		return;

	switch (guiscreen.type) {

		case SVGALIB:
			update_svga_screen();
			break;
#if 0
		case XLIB:
			update_X_screen();
			break;
#endif
	}

	guiscreen.x_min = -1;
	guiscreen.x_max = -1;
	guiscreen.y_min = -1;
	guiscreen.y_max = -1;
}


int determine_clip(int *x1, int *y1, int *x2, int *y2)
{
	if (_clip) {	/* clipping enabled */
		if (*x1 > _clipx2 || *y1 > _clipy2)
			return FALSE;
		if (*x2 < _clipx1 || *y2 < _clipy1)
			return FALSE;
		if (*x1 < _clipx1)
			*x1 = _clipx1;
		if (*x2 > _clipx2)	
			*x2 = _clipx2;
		if (*y1 < _clipy1)
			*y1 = _clipy1;
		if (*y2 > _clipy2)
			*y2 = _clipy2;
	}
	return TRUE;
}


void copy_window_to_screen_image(GuiWindow *win, int savescreen_, int update_mouse, int show_other)
{
	GuiWindow *tmp_win;
	GuiWinThread *win_thread;
	int x1, x2, y1, y2;
	int line, length, mouse_in_window = FALSE, overlap, z;
	char *data, *screen_data;

	check_window(win, "copy_window_to_screen_image");
	win_thread = win->win_thread;
	x1 = win->x + win->x_min;
	x2 = win->x + win->x_max;
	y1 = win->y + win->y_min;
	y2 = win->y + win->y_max;

	if (win->x_min == -1 || win->x_max == -1 || win->y_min == -1 || win->y_max == -1)
		return;
	if (!determine_clip(&x1, &y1, &x2, &y2))	/* if outside return */
		return;
	if (mouse.x + cursor[mouse.cursor_id].cur_width >= x1 && mouse.x <= x2 &&
	    mouse.y + cursor[mouse.cursor_id].cur_height >= y1 && mouse.y <= y2 && update_mouse) {
		mouse_in_window = TRUE;
		delete_mouse();
	}

	if (savescreen_) {
		data = win_thread->background + x1 + y1 * guiscreen.width;
		length = guiscreen.width;
	} else {
		data = win->data + x1 - win->x + (y1 - win->y) * win->width;
		length = win->width;
	}
	screen_data = guiscreen.data + x1 + y1 * guiscreen.width;
	for (line = 0; line < y2 - y1 + 1; line++) {
		memcpy(screen_data, data, x2 - x1 + 1);
		data += length;
		screen_data += guiscreen.width;
	}
	set_screen_update(x1, y1, x2, y2);

	/* reset the update region of the window */
	win->x_min = -1;
	win->x_max = -1;
	win->y_min = -1;
	win->y_max = -1;

	if (!show_other) {
		if (mouse_in_window)
			show_mouse();
		return;
	}
	
	/* redraw the other windows */
	if (savescreen_)
		z = (win->win_thread)->number - 1;
	else 
		z = win->z_order - 1;
	set_clipping_window(x1, y1, x2, y2);
	while (z >= 0) {
		tmp_win = win_thread->first;
		while (tmp_win->z_order != z) {
			tmp_win = tmp_win->next;
			//if (tmp_win == NULL)
				//fprintf(stderr, "---> Warning: window (z = %d) does not exist.\n", z);
		}
		if (tmp_win != NULL && !tmp_win->hide && tmp_win != win) {
			x1 = tmp_win->x;
			y1 = tmp_win->y;
			x2 = tmp_win->x + tmp_win->width - 1;
			y2 = tmp_win->y + tmp_win->height - 1;
			overlap = determine_clip(&x1, &y1, &x2, &y2);
			if (overlap) {
				data = tmp_win->data + x1 - tmp_win->x + (y1 - tmp_win->y) * tmp_win->width;
				screen_data = guiscreen.data + x1 + y1 * guiscreen.width;
				for (line = 0; line < y2 - y1 + 1; line++) {
					memcpy(screen_data, data, x2 - x1 + 1);
					data += tmp_win->width;
					screen_data += guiscreen.width;
				}
			}
		}
		z--;
	}
	set_clipping_window(0, 0, guiscreen.width - 1, guiscreen.height - 1);

	if (mouse_in_window)
		show_mouse();
}


void set_screen_update(int x_min, int y_min, int x_max, int y_max)
{
	if (guiscreen.x_min == -1 || guiscreen.x_max == -1 ||
	    guiscreen.y_min == -1 || guiscreen.y_max == -1) {
	    	guiscreen.x_min = x_min;
	    	guiscreen.x_max = x_max;
	    	guiscreen.y_min = y_min;
	    	guiscreen.y_max = y_max;
	} else {
		if (x_min < guiscreen.x_min)
			guiscreen.x_min = x_min;
		if (x_max > guiscreen.x_max)
			guiscreen.x_max = x_max;
		if (y_min < guiscreen.y_min)
			guiscreen.y_min = y_min;
		if (y_max > guiscreen.y_max)
			guiscreen.y_max = y_max;
	}
}


void close_screen(void)
{
	switch (guiscreen.type) {

		case SVGALIB:
			close_svga_screen();
			break;
#if 0
		case XLIB:
			close_X_screen();
			break;
#endif
	}
}


void set_default_palette(void)
{
	switch (guiscreen.type) {

		case SVGALIB:
			set_svga_default_palette(guiscreen.colors);
			break;
#if 0
		case XLIB:
			set_X_default_palette(guiscreen.colors);
			break;
#endif
	}
}


void set_gui_palette(int color, int red, int green, int blue)
{
	switch (guiscreen.type) {

		case SVGALIB:
			set_svga_palette(color, red, green, blue);
			break;
#if 0
		case XLIB:
			set_X_palette(color, red, green, blue);
			break;
#endif
	}
}


void get_gui_palette(int color, int *red, int *green, int *blue)
{
	switch (guiscreen.type) {

		case SVGALIB:
			get_svga_palette(color, red, green, blue);
			break;
#if 0
		case XLIB:
			get_X_palette(color, red, green, blue);
			break;
#endif
	}
}
