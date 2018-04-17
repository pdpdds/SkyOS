/*************************************************************************
	button.c -- last change: 20-1-1998

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

#include "windef.h"
#include <stdio.h>
//#include <stdlib.h>
#include "string.h"
#include "sprintf.h"
#include "local.h"
#include "memory.h"
#include "images/circle.xpm"
#include "images/check1.xpm"
#include "images/check2.xpm"
#include "images/radio1.xpm"
#include "images/radio2.xpm"


void create_button(GuiObject * obj)
{
	int count, number, color;
	int x_off = 0, y_off = 1, pos;

	check_object(obj, "obj", "create_button");
	number = obj->width * obj->height;

	/* define the two pixmaps for the buttons */
	for (count = 0; count < 2; count++) {
		color = count ? obj->bg_col2 : obj->bg_col1;
		if (obj->type == LIGHT_BUTTON)
			color = obj->bg_col1;
		if (obj->type == RADIO_BUTTON || obj->type == CHECK_BUTTON)
			color = (obj->win)->bg_col;
		memset(obj->data[count], color, number);
	}
	if (strlen(obj->label) > 0) {
		x_off = get_object_align(obj, string_length(obj->label));
		y_off = (obj->height - 10) / 2;
		if (y_off < 2)
			y_off = 2;
	}
	if (obj->type == LIGHT_BUTTON) {
		pos = (obj->height - 9) / 2;
		set_object_image_data(obj, circle_xpm, 4, pos, FALSE, color);
		set_object_image_data(obj, circle_xpm, 5, pos + 1, TRUE, color);
		x_off = get_object_align(obj, string_length(obj->label) + 13) + 13;
	}
	if (obj->type == RADIO_BUTTON) {
		set_object_image_data(obj, radio1_xpm, 1, 1, FALSE, color);
		set_object_image_data(obj, radio2_xpm, 1, 1, TRUE, color);
		x_off += 17;
	}
	if (obj->type == CHECK_BUTTON) {
		set_object_image_data(obj, check1_xpm, 0, 0, FALSE, color);
		set_object_image_data(obj, check2_xpm, 0, 0, TRUE, color);
		x_off += 17;
	}
	if (strlen(obj->label) > 0) {
		string_to_object(obj, x_off, y_off, 0, obj->active ? NORMAL_TEXT : EMBOSSED_TEXT);
		if (obj->type != CHECK_BUTTON && obj->type != RADIO_BUTTON) {
			x_off++;
			y_off++;
		}
		string_to_object(obj, x_off, y_off, 1, obj->active ? NORMAL_TEXT : EMBOSSED_TEXT);
	}
}


GuiObject *add_button(GuiWindow * win, int type, int x, int y,
		      int width, int height, char *label)
{
	GuiObject *obj;
	int count;
	
	check_window(win, "add_button");

	obj = (GuiObject *) malloc(sizeof(GuiObject));
	if (obj == NULL)
		error("Cannot allocate memory for button.");

	obj->win = win;
	obj->x = obj->x_min = x;
	obj->y = obj->y_min = y;
	if (type == CHECK_BUTTON || type == RADIO_BUTTON) {
		width = string_length(label) + 20;
		height = 16;
	}
	obj->width = width;
	obj->height = height;
	obj->pressed = FALSE;
	obj->active = TRUE;
	obj->hide = FALSE;
	obj->wait_for_mouse = TRUE;
	obj->objclass = BUTTON;
	obj->type = type;
	obj->fg_col = BUTTON_FORE;	/* foreground */
	obj->bg_col1 = BUTTON_BACK;	/* background not pressed */
	obj->bg_col2 = BUTTON_BACK;	/* background pressed */
	obj->u_data = 0;
	sprintf(obj->label, "%s", label);
	if (obj->type == CHECK_BUTTON || obj->type == RADIO_BUTTON) {
		obj->frame_type = NO_FRAME;
		obj->align = ALIGN_LEFT;
		obj->bg_col1 = WIN_BACK;
		obj->fg_col = TEXT_NORMAL;
	} else {
		obj->frame_type = UP_FRAME;
		obj->align = ALIGN_CENTER;
		if (obj->type == LIGHT_BUTTON)
			obj->bg_col2 = GREEN;
	}
	obj->x_max = x + width - 1;
	obj->y_max = y + height - 1;
	obj->info[0] = '\0';	/* make string length zero */
	set_object_callback(obj, object_cb);

	for (count = 0; count < 2; count++) {
		obj->data[count] = (char *) malloc(obj->width * obj->height);
		if (obj->data[count] == NULL)
			error("Unable to allocate data array in add_button().");
	}
	create_button(obj);
	add_object(obj);

	return obj;
}


void update_button(GuiObject * obj)
{
	check_object(obj, "obj", "update_button");
	check_window(obj->win, "update_button");

	if (obj->hide)
		return;
	
	win_object(obj);
	if (obj->frame_type) {
		if (obj->pressed)
			win_3dbox(obj->win, DOWN_FRAME, obj->x, obj->y, obj->width, obj->height);
		else
			win_3dbox(obj->win, UP_FRAME, obj->x, obj->y, obj->width, obj->height);
	}
}


void show_button(GuiObject * obj)
{
	check_object(obj, "obj", "show_button");
	check_window(obj->win, "show_button");

	obj->pressed = obj->pressed ? FALSE : TRUE;

	update_button(obj);
	show_window(obj->win);
}


void set_button_active(GuiObject *obj, int active)
{
	check_object(obj, "obj", "set_button_active");

	obj->active = active;
	create_button(obj);
	update_button(obj);
}
