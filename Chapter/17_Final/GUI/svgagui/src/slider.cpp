/*************************************************************************
	slider.c -- last change: 8-1-2000

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

#include "stdio.h"
#include "SkyAPI.h"
#include "sprintf.h"
#include "local.h"
#include "windef.h"
#include "PIT.h"

#include "images/up.xpm"
#include "images/down.xpm"
#include "images/left.xpm"
#include "images/right.xpm"

#define dimension 13
#define slid_width 17


static void new_position(GuiObject *slid, int pos_neg)
{
	int old_position;

	check_object(slid, "slid", "new_position");

	old_position = slid->position;
	if (pos_neg < -1)
		slid->position -= slid->length;
	else if (pos_neg < 0)
		slid->position--;
	else if (pos_neg > 1)
		slid->position += slid->length;
	else if (pos_neg > 0)
		slid->position++;

	if (slid->position < 0)
		slid->position = 0;
	if (slid->position > slid->slider_length - slid->length)
		slid->position = slid->slider_length - slid->length;

	if (old_position != slid->position) {
		show_slider(slid);
		slid->object_callback(slid, slid->u_data);
	}
}


static void move_slider(GuiObject * slid, int pos_neg)
{
	GuiWinThread *win_thread;

	check_object(slid, "slid", "move_slider");
	check_window(slid->win, "move_slider");
	win_thread = (slid->win)->win_thread;

	do {
		//usleep(sleep_time);
		do_window_functions(win_thread);
		new_position(slid, pos_neg);
		if (GuiGetMessage() == GuiMouseEvent)
			move_mouse();
	} while (GuiMouseGetButton() == GuiMouseLeftButton);
}


static void up_cb(GuiObject * obj, int data)
{
	check_object(obj, "obj", "up_cb");

	new_position(obj->obj_link, 1);
}


static void down_cb(GuiObject * obj, int data)
{
	check_object(obj, "obj", "down_cb");

	new_position(obj->obj_link, -1);
}


static void left_cb(GuiObject * obj, int data)
{
	check_object(obj, "obj", "left_cb");

	new_position(obj->obj_link, -1);
}


static void right_cb(GuiObject * obj, int data)
{
	check_object(obj, "obj", "right_cb");

	new_position(obj->obj_link, 1);
}


GuiObject *add_slider(GuiWindow * win, int type, int x, int y, int length,
	int buttons)
{
	GuiObject *slid, *obj;

	check_window(win, "add_slider");

	slid = (GuiObject *)malloc(sizeof(GuiObject));
	if (slid == NULL)
		error("Cannot allocate memory for slider.");

	slid->win = win;
	slid->x = x;
	slid->y = y;
	slid->active = TRUE;
	slid->pressed = FALSE;
	slid->hide = FALSE;
	slid->slider_length = length;
	slid->position = 0;
	slid->objclass = SLIDER;
	slid->type = type;
	slid->bg_col1 = WIN_BACK;	/* background */
	slid->bg_col2 = SLIDER_BACK;	/* slider head */
	slid->fg_col = SLIDER_FORE;	/* slider middle */
	slid->u_data = 0;
	slid->info[0] = '\0';	/* make string length zero */
	set_object_callback(slid, object_cb);
	set_object_callback2(slid, object_cb);

	slid->data[0] = NULL;	/* don't use the data blocks */
	slid->data[1] = NULL;

	/* only used for type VERT_SLIDER and HOR_SLIDER */
	if (type == HOR_SLIDER || type == VERT_SLIDER) {
		slid->buttons = buttons;
		slid->length = 10;
	}
	/* only used for type NICE_HOR_SLIDER, NICE_VERT_SLIDER and NICE_VALUE_SLIDER */
	slid->middle = FALSE;

	if (slid->type == VERT_SLIDER) {
		slid->x_min = slid->x;
		slid->x_max = slid->x + dimension - 1;
		slid->y_min = slid->y;
		slid->y_max = slid->y + slid->slider_length - 1;

		if (slid->buttons) {
			obj = add_button(win, PIXMAP_BUTTON, slid->x_min,
				slid->y_min - dimension, dimension,
				dimension, "");
			set_object_image_data(obj, up_xpm, 3, 4, 0, obj->bg_col1);
			set_object_image_data(obj, up_xpm, 4, 5, 1, obj->bg_col1);
			set_object_callback(obj, up_cb);
			obj->wait_for_mouse = FALSE;
			obj->obj_link = slid;

			obj = add_button(win, PIXMAP_BUTTON, slid->x_min,
				slid->y_max + 1, dimension, dimension,
				"");
			set_object_image_data(obj, down_xpm, 3, 4, 0, obj->bg_col1);
			set_object_image_data(obj, down_xpm, 4, 5, 1, obj->bg_col1);
			set_object_callback(obj, down_cb);
			obj->wait_for_mouse = FALSE;
			obj->obj_link = slid;
		}
	}
	if (slid->type == HOR_SLIDER) {
		slid->x_min = slid->x;
		slid->x_max = slid->x + slid->slider_length - 1;
		slid->y_min = slid->y;
		slid->y_max = slid->y + dimension - 1;
		if (slid->buttons) {
			obj = add_button(win, PIXMAP_BUTTON, slid->x_min - dimension,
				slid->y_min, dimension, dimension, "");
			set_object_image_data(obj, left_xpm, 4, 3, 0, obj->bg_col1);
			set_object_image_data(obj, left_xpm, 5, 4, 1, obj->bg_col1);
			set_object_callback(obj, left_cb);
			obj->wait_for_mouse = FALSE;
			obj->obj_link = slid;

			obj = add_button(win, PIXMAP_BUTTON, slid->x_max + 1,
				slid->y_min, dimension, dimension, "");
			set_object_image_data(obj, right_xpm, 5, 3, 0, obj->bg_col1);
			set_object_image_data(obj, right_xpm, 6, 4, 1, obj->bg_col1);
			set_object_callback(obj, right_cb);
			obj->wait_for_mouse = FALSE;
			obj->obj_link = slid;
		}
	}
	if (slid->type == NICE_HOR_SLIDER) {
		slid->x_min = slid->x - 4;
		slid->x_max = slid->x + slid->slider_length + 4;
		slid->y_min = slid->y;
		slid->y_max = slid->y + 15;
		slid->length = 1;
	}
	if (slid->type == NICE_VERT_SLIDER || slid->type == NICE_VALUE_SLIDER) {
		slid->x_min = slid->x;
		slid->x_max = slid->x + 15;
		slid->y_min = slid->y - 4;
		slid->y_max = slid->y + slid->slider_length + 4;
		slid->length = 1;
	}
	add_object(slid);	/* add slider to the object list */

	if (slid->type == NICE_VALUE_SLIDER) {
		add_text(win, NORMAL_TEXT, slid->x_min - 18, slid->y - 18, "0.00");
		set_slider_maxvalue(slid, 1.0);
	}
	return slid;
}


static void update_slider_value(GuiObject * slid)
{
	GuiObject *text;
	GuiWindow *win;

	check_object(slid, "slid", "update_slider_value");
	text = slid->next;
	check_object(text, "text", "update_slider_value");
	win = slid->win;
	check_window(win, "update_slider_value");

	if (slid->max_value <= 2)
		sprintf(text->label, "%0.2f", slid->position / (float)(slid->slider_length - 1) *
			slid->max_value);
	else if (slid->max_value <= 10)
		sprintf(text->label, "%0.1f", slid->position / (float)(slid->slider_length - 1) *
			slid->max_value);
	else
		sprintf(text->label, "%0.0f", slid->position / (float)(slid->slider_length - 1) *
			slid->max_value);

	win_fillbox(win, text->x, text->y, text->width, text->height, text->bg_col1);
	text->x = slid->x_min - string_length(text->label) / 2 + slid_width / 2 - 1;
	create_text(text);
	update_text(text);
}


void create_slider(GuiObject * slid)
{
	GuiWindow *win;
	int x, y, length, XPos, YPos;

	check_object(slid, "slid", "create_slider");
	win = slid->win;
	check_window(win, "create_slider");

	if (slid->hide);
	else if (slid->type == HOR_SLIDER) {	/* horizontal slider */
		win_fillbox(win, slid->x, slid->y, slid->slider_length, dimension, slid->bg_col2);
		win_fillbox(win, slid->x + slid->position, slid->y, slid->length, dimension, slid->fg_col);
		win_3dbox(win, UP_FRAME, slid->x + slid->position, slid->y, slid->length, dimension);
	}
	else if (slid->type == VERT_SLIDER) {		/* vertical slider */
		win_fillbox(win, slid->x, slid->y, dimension, slid->slider_length, slid->bg_col2);
		win_fillbox(win, slid->x, slid->y + slid->slider_length - slid->position -
			slid->length, dimension, slid->length, slid->fg_col);
		win_3dbox(win, UP_FRAME, slid->x, slid->y + slid->slider_length - slid->position -
			slid->length, dimension, slid->length);
	}
	else if (slid->type == NICE_HOR_SLIDER) {	/* nice horizontal slider */
		win_fillbox(win, slid->x - 4, slid->y, slid->slider_length + 9, slid_width, slid->bg_col1);
		/* Draw a grid lines */
		for (x = 0; x <= slid->slider_length; x += 10) {
			if (x % 50 == 0)
				length = 3;
			else
				length = 1;
			XPos = slid->x + x;
			win_line(win, XPos, slid->y + 4, XPos, slid->y - length + 4, BLACK, FALSE);
			win_line(win, XPos, slid->y + 12, XPos, slid->y + 12 + length, BLACK, FALSE);
		}
		/* Draw the slider */
		win_box(win, slid->x, slid->y + slid_width / 2, slid->slider_length, 1, BLACK);
		win_3dbox(win, DOWN_FRAME, slid->x - 1, slid->y + slid_width / 2 - 2, slid->slider_length + 2, 5);
		if (slid->middle)
			win_box(win, slid->x + (slid->slider_length) / 2, slid->y + slid_width / 2 - 2, 0, 6, RED);
		win_fillbox(win, slid->x + slid->position - 4, slid->y, 8, slid_width, slid->bg_col2);
		win_3dbox(win, UP_FRAME, slid->x + slid->position - 4, slid->y, 8, slid_width);
		win_box(win, slid->x + slid->position, slid->y + 3, 0, slid_width - 6, slid->fg_col);
	}
	else if (slid->type == NICE_VERT_SLIDER ||
		slid->type == NICE_VALUE_SLIDER) {	/* nice vertical slider */
											/* Draw a grid lines */
		win_fillbox(win, slid->x, slid->y - 4, slid_width, slid->slider_length + 9, slid->bg_col1);
		for (y = 0; y <= slid->slider_length; y += 10) {
			if (y % 50 == 0)
				length = 3;
			else
				length = 1;
			YPos = slid->y + slid->slider_length - y - 1;
			win_line(win, slid->x + 12, YPos, slid->x + 12 + length, YPos, BLACK, FALSE);
			win_line(win, slid->x + 3 - length, YPos, slid->x + 3, YPos, BLACK, FALSE);
		}
		/* Draw the slider */
		win_box(win, slid->x + slid_width / 2, slid->y, 1, slid->slider_length, BLACK);
		win_3dbox(win, DOWN_FRAME, slid->x + slid_width / 2 - 2, slid->y - 1, 5, slid->slider_length + 2);
		if (slid->middle)
			win_box(win, slid->x + slid_width / 2 - 2, (slid->slider_length) / 2 + slid->y, 6, 0, RED);
		win_fillbox(win, slid->x, slid->slider_length - slid->position + slid->y - 4, slid_width, 8, slid->bg_col2);
		win_3dbox(win, UP_FRAME, slid->x, slid->slider_length - slid->position + slid->y - 4, slid_width, 8);
		win_box(win, slid->x + 3, slid->slider_length - slid->position + slid->y, slid_width - 6, 0, slid->fg_col);
	}
	if (slid->type == NICE_VALUE_SLIDER)
		update_slider_value(slid);
}


void update_slider(GuiObject * obj)
{
	check_object(obj, "obj", "update_slider");

	create_slider(obj);
}


void set_slider_position(GuiObject * slid, int position)
{
	check_object(slid, "slid", "set_slider_position");

	slid->position = position;
	create_slider(slid);
}


void set_slider_barlength(GuiObject * slid, int length)
{
	check_object(slid, "slid", "set_slider_barlength");

	slid->length = length;
	create_slider(slid);
}


void set_slider_maxvalue(GuiObject * slid, float max)
{
	check_object(slid, "slid", "set_slider_maxvalue");

	if (max > 999)
		max = 999;
	slid->max_value = max;
	create_slider(slid);
}


void show_slider(GuiObject * slid)
{
	check_object(slid, "slid", "show_slider");
	check_window(slid->win, "show_slider");

	update_slider(slid);
	show_window(slid->win);
}


void press_slider(GuiObject * slid)
{
	GuiWindow *win;
	GuiWinThread *win_thread;
	int position, old_x, old_y;
	int x, y;
	int old_position = 0, on_bar = FALSE;

	check_object(slid, "slid", "press_slider");
	win = slid->win;
	check_window(win, "press_slider");
	win_thread = win->win_thread;
	old_x = slid->x;
	old_y = slid->y;
	x = mouse.x - win->x;
	y = mouse.y - win->y;

	if ((slid->type == HOR_SLIDER &&	/* on slider bar? */
		x >= slid->x + slid->position &&
		x <= slid->x + slid->position + slid->length) ||
		(slid->type == VERT_SLIDER &&
			y >= slid->y + slid->slider_length - slid->position - slid->length &&
			y <= slid->y + slid->slider_length - slid->position)) {
		old_x = x;
		old_y = y - slid->slider_length;
		old_position = slid->position;
		on_bar = TRUE;
	}
	if (!on_bar && (slid->type == HOR_SLIDER || slid->type == VERT_SLIDER)) {
		if (slid->type == HOR_SLIDER) {
			if (x > slid->x + slid->position)
				move_slider(slid, 2);
			else
				move_slider(slid, -2);
		}
		if (slid->type == VERT_SLIDER) {
			if (y > slid->y + slid->slider_length - slid->position)
				move_slider(slid, -2);
			else
				move_slider(slid, +2);
		}
		return;
	}
	do {
		//usleep(sleep_time);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent) {
			move_mouse();
			x = mouse.x - win->x;
			y = mouse.y - win->y;
			position = old_position;
			if (slid->type == HOR_SLIDER || slid->type == NICE_HOR_SLIDER)
				position += x - old_x;
			else
				position += slid->slider_length - (y - old_y);
			if (position > slid->slider_length - slid->length)
				position = slid->slider_length - slid->length;
			if (position < 0)
				position = 0;
			slid->position = position;
			show_slider(slid);
			slid->object_callback(slid, slid->u_data);
		}
	} while (GuiMouseGetButton() == GuiMouseLeftButton);

	/* check for double click on slider bar */
	if (on_bar)
		check_double_click(slid);
}
