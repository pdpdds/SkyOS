/*************************************************************************
	window_draw.c -- last change: 20-1-1998

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
#include <string.h>
#include "memory.h"
#include "math.h"
#include "local.h"
#include "windef.h"

#define SWAP(x,y) do { int temp = (int)(x); (x) = (int)(y); (y) = temp; } while(0)
#define ORDER(x,y) do { if ((int)(y) < (int)(x)) SWAP((x),(y)); } while(0)

void set_update_region(GuiWindow * win, int x_min, int y_min, int x_max, int y_max)
{
	check_window(win, "set_update_region");

	if (x_min < win->x_min || win->x_min == -1)
		win->x_min = x_min;
	if (x_max > win->x_max || win->x_max == -1)
		win->x_max = x_max;
	if (y_min < win->y_min || win->y_min == -1)
		win->y_min = y_min;
	if (y_max > win->y_max || win->y_max == -1)
		win->y_max = y_max;
}


static void vert_win_line(GuiWindow * win, int x, int y1, int y2, int color, int update_region)
{
	int w;

	check_window(win, "vert_win_line");
	w = win->width;

	if (!win->data)
		return;
	if ((x < 0) || (x > w - 1))
		return;
	ORDER(y1, y2);
	if ((y1 > win->height - 1) || (y2 < 0))
		return;
	if (y1 < 0)
		y1 = 0;
	if (y2 > win->height - 1)
		y2 = win->height - 1;
	if (update_region)
		set_update_region(win, x, y1, x, y2);
	{
		char *p = win->data + x + w * y1;
		char *end = win->data + x + w * y2;
		unsigned char c = color;

		for (; p != end; p += w)
			*p = c;
		*p = c;
	}
}


static void hor_win_line(GuiWindow * win, int x1, int x2, int y, int color, int update_region)
{
	char *p;

	check_window(win, "hor_win_line");

	if (!win->data)
		return;
	if ((y < 0) || (y > win->height - 1))
		return;
	ORDER(x1, x2);
	if ((x1 > win->width - 1) || (x2 < 0))
		return;
	if (x1 < 0)
		x1 = 0;
	if (x2 > win->width - 1)
		x2 = win->width - 1;
	if (update_region)
		set_update_region(win, x1, y, x2, y);
	p = win->data + x1 + win->width * y;
	memset(p, color, x2 - x1 + 1);
}


static void any_win_line(GuiWindow * win, int x1, int y1, int x2, int y2, int color, int update_region)
{
	int pos, x_min, x_max, y_min, y_max, x, y;
	int x1_, x2_, y1_, y2_;
	float step, line;
	
	check_window(win, "any_win_line");

	if (!win->data)
		return;
	if (update_region) {
		x1_ = (x1 < 0) ? 0 : x1;
		x1_ = (x1_ > win->width - 1) ? win->width - 1 : x1_;
		x2_ = (x2 < 0) ? 0 : x2;
		x2_ = (x2_ > win->width - 1) ? win->width - 1 : x2_;
		x_min = (x2_ < x1_) ? x2_ : x1_;
		x_max = (x1_ < x2_) ? x2_ : x1_;
		y1_ = (y1 < 0) ? 0 : y1;
		y1_ = (y1_ > win->height - 1) ? win->height - 1 : y1_;
		y2_ = (y2 < 0) ? 0 : y2;
		y2_ = (y2_ > win->height - 1) ? win->height - 1 : y2_;
		y_min = (y2_ < y1_) ? y2_ : y1_;
		y_max = (y1_ < y2_) ? y2_ : y1_;
		set_update_region(win, x_min, y_min, x_max, y_max);
	}
	step = (y2 - y1) / (float) (x2 - x1);
	if (fabs(step) <= 1.0) {
		line = y1;
		for (x = x1; x <= x2; x++) {
			line += step;
			y = (int)line;
			if (x >= 0 && x < win->width &&
			    y >= 0 && y < win->height) {
				pos = x + y * win->width;
				win->data[pos] = color;
			}
		}
	} else {
		step = 1.0 / step;
		line = x1;
		for (y = y1; y <= y2; y++) {
			line += step;
			x = (int)line;
			if (x >= 0 && x < win->width &&
			    y >= 0 && y < win->height) {
				pos = x + y * win->width;
				win->data[pos] = color;
			}
		}
	}
}

void win_line(GuiWindow * win, int x1, int y1, int x2, int y2, int color, int update_region)
{
	if (x1 == x2) {
		vert_win_line(win, x1, y1, y2, color, update_region);
		return;
	} else if (y1 == y2) {
		hor_win_line(win, x1, x2, y1, color, update_region);
		return;
	} else {
		any_win_line(win, x1, y1, x2, y2, color, update_region);
		return;
	}
}


void win_3dline(GuiWindow * win, int x1, int y1, int x2, int y2)
{
	if (x1 == x2) {
		vert_win_line(win, x1, y1, y2, TEXT_EMBOSS, TRUE);
		vert_win_line(win, x1 + 1, y1, y2, WHITE, TRUE);
		return;
	} else if (y1 == y2) {
		hor_win_line(win, x1, x2, y1, TEXT_EMBOSS, TRUE);
		hor_win_line(win, x1, x2, y1 + 1, WHITE, TRUE);
		return;
	}
}


void win_box(GuiWindow * win, int x, int y, int width, int height, int color)
{
	win_line(win, x, y, x + width - 1, y, color, TRUE);
	win_line(win, x, y + height - 1, x + width - 1, y + height - 1, color, TRUE);
	win_line(win, x, y, x, y + height - 1, color, TRUE);
	win_line(win, x + width - 1, y, x + width - 1, y + height - 1, color, TRUE);
}


void win_fillbox(GuiWindow * win, int x, int y, int width, int height, int color)
{
	int line;

	for (line = 0; line < height; line++)
		win_line(win, x, y + line, x + width - 1, y + line, color, TRUE);
}


void win_3dbox(GuiWindow * win, int type, int x, int y, int width, int height)
{
	int color = TEXT_EMBOSS, color2 = WHITE;

	if (type == UP_FRAME || type == DOWN_FRAME) {
		color = (type == DOWN_FRAME) ? BLACK : WHITE;
		color2 = (type == DOWN_FRAME) ? DARKGREY : GREY;
		win_line(win, x, y, x + width - 1, y, color, TRUE);
		win_line(win, x + 1, y + 1, x + width - 2, y + 1, color2, TRUE);
		win_line(win, x, y, x, y + height - 1, color, TRUE);
		win_line(win, x + 1, y + 1, x + 1, y + height - 2, color2, TRUE);
		color = (type == DOWN_FRAME) ? WHITE : BLACK;
		color2 = (type == DOWN_FRAME) ? GREY : DARKGREY;
		win_line(win, x, y + height - 1, x + width - 1, y + height - 1, color, TRUE);
		win_line(win, x + 1, y + height - 2, x + width - 1, y + height - 2, color2, TRUE);
		win_line(win, x + width - 1, y, x + width - 1, y + height - 1, color, TRUE);
		win_line(win, x + width - 2, y + 1, x + width - 2, y + height - 2, color2, TRUE);
	}
	if (type == FLAT_FRAME) {
		win_line(win, x, y, x + width - 1, y, color, TRUE);
		win_line(win, x + 1, y + 1, x + width - 1, y + 1, color2, TRUE);
		win_line(win, x, y, x, y + height - 2, color, TRUE);
		win_line(win, x + 1, y + 1, x + 1, y + height - 3, color2, TRUE);
		win_line(win, x, y + height - 2, x + width - 2, y + height - 2, color, TRUE);
		win_line(win, x, y + height - 1, x + width - 1, y + height - 1, color2, TRUE);
		win_line(win, x + width - 2, y + 2, x + width - 2, y + height - 2, color, TRUE);
		win_line(win, x + width - 1, y + 1, x + width - 1, y + height - 1, color2, TRUE);
	}
}


void win_pixmap(GuiObject * obj, int x_pos, int y_pos, int element, int active)
{
	GuiWindow *win;
	int i, j;
	int w, h, w_;
	char *source, *dest;

	check_object(obj, "obj", "win_pixmap");
	win = obj->win;
	check_window(win, "win_pixmap");
	w = obj->width;
	h = obj->height;
	w_ = win->width;

	if ((x_pos < 0) || (x_pos > win->width) || (y_pos < 0) || (y_pos > win->height))
		return;
	if (win->data == NULL) {
//		fprintf(stderr, "---> Warning: win->data is NULL in win_pixmap().\n");
		return;
	}
	if (obj->data[element] == NULL) {
	//	fprintf(stderr, "---> Warning: obj->data[%d] is NULL in win_pixmap() for ", element);
		print_object_string(obj);
		return;
	}
	if (x_pos + obj->width > win->width)
		w = win->width - x_pos;
	if (y_pos + obj->height > win->height)
		h = win->height - y_pos;
	dest = win->data + x_pos + y_pos * win->width;
	source = obj->data[element];
	for (i = 0; i < h; i++) {
		memcpy(dest, source, w);
		dest += w_;
		source += obj->width;
	}

	/* not active => make grey */
	if (!active && obj->objclass != WINTEXT && obj->type != NORMAL_BUTTON &&
	    obj->type != LIGHT_BUTTON && obj->type != CHECK_BUTTON && obj->type != RADIO_BUTTON) {
		dest = win->data + x_pos + y_pos * win->width;
		for (i = 0; i < h; i++) {
			for (j = 0; j < w - 2; j += 2) {
				dest += 2;
				*dest = DARKGREY;
			}
			dest = win->data + x_pos + i % 2 + (y_pos + i) * win->width;
		}
	}
	set_update_region(win, x_pos, y_pos, x_pos + w - 1, y_pos + h - 1);
	
	if (0)
		win_box(win, obj->x_min, obj->y_min, obj->x_max - obj->x_min + 1, 
			obj->y_max - obj->y_min + 1, RED);
}


void win_object(GuiObject * obj)
{
		win_pixmap(obj, obj->x, obj->y, obj->pressed, obj->active);
}
