/*************************************************************************
	mouse.c -- last change: 20-1-1998

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
//#include <unistd.h>
#include "sprintf.h"
#include "string.h"
#include "memory.h"
#include "windef.h"
#include "PIT.h"
#include "local.h"
#include "images/pointer.xbm"
#include "images/pointer_mask.xbm"
#include "images/watch.xbm"
#include "images/watch_mask.xbm"


Mouse mouse;
GuiCursor cursor[10];


static void dummy_function(void)
{
}


void show_mouse(void)
{
	int x, y;
	int width = cursor[mouse.cursor_id].cur_width;
	int height = cursor[mouse.cursor_id].cur_height;
	char *mouse_data, *mask_data, *screen_data;

	if (guiscreen.type == XLIB)
		return;
	if (mouse.visible)
		return;
		
	if (mouse.x + width > guiscreen.width)
		width = guiscreen.width - mouse.x;
	if (mouse.y + height > guiscreen.height)
		height = guiscreen.height - mouse.y;

	/* save mouse background */
	mouse_data = mouse.savescreen;
	screen_data = guiscreen.data + (mouse.x - cursor[mouse.cursor_id].x_hot) +
		      (mouse.y - cursor[mouse.cursor_id].y_hot) * guiscreen.width;
	for (y = 0; y < height; y++) {
		memcpy(mouse_data, screen_data, width);
		mouse_data += width;
		screen_data += guiscreen.width;
	}

	/* show new mouse cursor */
	screen_data = guiscreen.data + (mouse.x - cursor[mouse.cursor_id].x_hot) +
		      (mouse.y - cursor[mouse.cursor_id].y_hot) * guiscreen.width;
	mouse_data = cursor[mouse.cursor_id].cur_bits;
	mask_data = cursor[mouse.cursor_id].mask_bits;
	for (y = 0;y < height;y++) {
		for (x = 0;x < width;x++) {
			if (*mask_data == (char)BLACK)
				*screen_data = *mouse_data;
			mouse_data++;
			mask_data++;
			screen_data++;
		}
		mouse_data += cursor[mouse.cursor_id].cur_width - width;
		mask_data += cursor[mouse.cursor_id].mask_width - width;
		screen_data += guiscreen.width - width;
	}
	x = mouse.x - cursor[mouse.cursor_id].x_hot;
	y = mouse.y - cursor[mouse.cursor_id].y_hot;
	set_screen_update(x, y, x + width - 1, y + height - 1);
	mouse.visible = TRUE;
}


void delete_mouse(void)
{
	int x, y, line;
	int width = cursor[mouse.cursor_id].cur_width;
	int height = cursor[mouse.cursor_id].cur_height;
	char *mouse_data, *screen_data;

	if (guiscreen.type == XLIB)
		return;
	if (!mouse.visible)
		return;
		
	if (mouse.x + width > guiscreen.width)
		width = guiscreen.width - mouse.x;
	if (mouse.y + height > guiscreen.height)
		height = guiscreen.height - mouse.y;
	/* delete old mouse cursor */
	mouse_data = mouse.savescreen;
	x = mouse.x - cursor[mouse.cursor_id].x_hot;
	y = mouse.y - cursor[mouse.cursor_id].y_hot;
	screen_data = guiscreen.data + x + y * guiscreen.width;
	for (line = 0; line < height; line++) {
		memcpy(screen_data, mouse_data, width);
		mouse_data += width;
		screen_data += guiscreen.width;
	}
	set_screen_update(x, y, x + width - 1, y + height - 1);
	mouse.visible = FALSE;
}


void reset_mouse(void)
{
	mouse.show_function = dummy_function;
	mouse.delete_function = dummy_function;

	switch_cursor(DEFAULT_CURSOR);
}


void init_mouse(void)
{
	if (mouse.num_cursors == 0) {
		add_new_cursor(pointer_bits, pointer_width, pointer_height, 
			   pointer_mask_bits, pointer_mask_width, pointer_mask_height,
			   0, 0);
		create_cursor(DEFAULT_CURSOR);
		add_new_cursor(watch_bits, watch_width, watch_height,
			   watch_mask_bits, watch_mask_width, watch_mask_height,
			   0, 0);
		create_cursor(WATCH_CURSOR);
	}
	switch (guiscreen.type) {
		case SVGALIB:
			init_svga_mouse();
			break;
#if 0
		case XLIB:
			init_X_mouse();
#endif
	}

	reset_mouse();
}


void move_mouse(void)
{
	delete_mouse();
	mouse.delete_function();
	switch (guiscreen.type) {
		case SVGALIB:
			get_svga_mouse_position();
			break;
#if 0
		case XLIB:
			get_X_mouse_position();
			break;
#endif
	}
	mouse.show_function();
	show_mouse();

	update_screen();
}


void set_mouse_position(int x, int y)
{
	switch (guiscreen.type) {
		case SVGALIB:
			set_svga_mouse_position(x, y);
			break;
#if 0
		case XLIB:
			set_X_mouse_position(x, y);
			break;
#endif
	}
}


void switch_cursor(int cursor_id)
{
	if (cursor_id > mouse.num_cursors - 1) {
		//fprintf(stderr, "----> Warning: cursor does not exist!\n");
		return;
	}
	if (mouse.cursor_id == cursor_id)
		return;
	delete_mouse();

	/* save screen under initial mouse position */
	if (mouse.savescreen != NULL)
		kfree(mouse.savescreen);
	mouse.savescreen = (char *) malloc(cursor[cursor_id].cur_width * cursor[cursor_id].cur_height);
	if (mouse.savescreen == NULL)
		error("Cannot allocate memory for mouse in switch_cursor().");

	switch (guiscreen.type) {
		case XLIB:
//			switch_X_cursor(cursor_id);
			break;
	}
	mouse.cursor_id = cursor_id;
	show_mouse();

	update_screen();
}


static void convert_bitmap_to_cursor(char *dest, char *source, int width, int height)
{
	int bit = 0;
	int pos = 0, i;
	
	for (i = 0;i < width * height;i++) {
		dest[i] = (source[pos] & (1 << bit)) ? BLACK : WHITE;
		bit++;
		if (bit > 7) {
			bit = 0;
			pos++;
		} else if ((i + 1) % width == 0) {
			pos++;
			bit = 0;
		}
	}	
}



int add_new_cursor(unsigned char *cur_bits, int cur_width, int cur_height,
		unsigned char *mask_bits, int mask_width, int mask_height,
		int x_hot, int y_hot)
{
	int cursor_id = mouse.num_cursors;

	cursor[cursor_id].cur_bits = (char *) malloc(cur_width * cur_height);
	cursor[cursor_id].mask_bits = (char *) malloc(mask_width * mask_height);
	if (cursor[cursor_id].cur_bits == NULL || cursor[cursor_id].mask_bits == NULL)
		error("Cannot allocate memory in add_new_cursor().");

	if (guiscreen.type == SVGALIB) {
		convert_bitmap_to_cursor(cursor[cursor_id].cur_bits, (char *)cur_bits,
					 cur_width, cur_height);
		convert_bitmap_to_cursor(cursor[cursor_id].mask_bits, (char *)mask_bits,
					 mask_width, mask_height);
	} else {
		memcpy(cursor[cursor_id].cur_bits, cur_bits, cur_width * cur_height);
		memcpy(cursor[cursor_id].mask_bits, mask_bits, mask_width * mask_height);
	}
	cursor[cursor_id].cur_width = cur_width;
	cursor[cursor_id].cur_height = cur_height;
	cursor[cursor_id].mask_width = mask_width;
	cursor[cursor_id].mask_height = mask_height;
	cursor[cursor_id].x_hot = x_hot;
	cursor[cursor_id].y_hot = y_hot;

	mouse.num_cursors++;

	return cursor_id;
}


void create_cursor(int cursor_id)
{
	if (cursor_id > mouse.num_cursors - 1) {
//		fprintf(stderr, "----> Warning: cursor does not exist!\n");
		return;
	}
	switch (guiscreen.type) {
		case XLIB:
//			create_X_cursor(cursor_id);
			break;
	}
}
