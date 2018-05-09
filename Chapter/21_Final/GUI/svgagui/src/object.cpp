/*************************************************************************
	object.c -- last change: 6-9-1998

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
#include "string.h"
#include "windef.h"
#include "PIT.h"

#include "local.h"

void print_object_string(GuiObject *obj)
{
	GuiObject *title;
	char objname[12][12] = {"Button", "Slider", "Text", "Number", "Input",
			       "Image", "Browser", "Pulldown", "Item", "Choice",
			       "Listbox", "Listentry"};
	
	check_object(obj, "obj", "print_object_string");
	check_window(obj->win, "print_object_string");
	title = (obj->win)->title;
	
	/*if (title != NULL)
		fprintf(stderr, "%s [%s] (win = %s).\n", objname[obj->objclass], obj->label, title->label);
	else
		fprintf(stderr, "%s [%s] (win = unknown).\n", objname[obj->objclass], obj->label);*/
}


void object_region(GuiWindow * win)
{
	int found = FALSE;
	GuiObject *object;

	check_window(win, "object_region");
	object = win->first;

	if (!win->active) {
		win->focusobj = NULL;
		return;
	}

	while (object != NULL && !found) {
		if (mouse.x >= object->x_min + win->x &&
		    mouse.x <= object->x_max + win->x &&
		    mouse.y >= object->y_min + win->y &&
		    mouse.y <= object->y_max + win->y &&
		    !object->hide && object->objclass != LISTBOX && object->objclass != ICONLIST)
			found = TRUE;
		if (!found)
			object = object->next;
	}
	if (found)
		win->focusobj = object;
	else
		win->focusobj = NULL;
}


void object_cb(GuiObject * obj, int data)
{
}


void add_object(GuiObject * obj)
{
	GuiWindow *win;
	GuiObject *object;

	check_object(obj, "obj", "add_object");
	win = obj->win;
	check_window(win, "add_object");
	object = win->first;

	if (object == NULL) {
		win->first = obj;
		obj->next = NULL;
		obj->prev = NULL;
	} else {
		while (object->next != NULL)
			object = object->next;
		object->next = obj;
		obj->next = NULL;
		obj->prev = object;
	}
}


void delete_object(GuiObject * obj)
{
	GuiWindow *win;
	
	if (check_object(obj, "obj", "delete_object"))
		return;
	win = obj->win;
	check_window(win, "delete_object");

	if (win->first == obj) {	/* first element */
		if (obj->next != NULL) {
			win->first = obj->next;
			(obj->next)->prev = NULL;
		} else
			win->first = NULL;
	} else if (obj->next == NULL)	/* last element */
		(obj->prev)->next = NULL;
	else {
		(obj->next)->prev = obj->prev;
		(obj->prev)->next = obj->next;
	}
	if (obj->data[0] != NULL)
		free(obj->data[0]);
	if (obj->data[1] != NULL)
		free(obj->data[1]);
	free(obj);
}


void hide_object(GuiObject * obj, int hide)
{
	check_object(obj, "obj", "hide_object");

	obj->hide = hide;
}


void update_object(GuiObject * object)
{
	check_object(object, "object", "update_object");

	switch (object->objclass) {
	case BUTTON:
		update_button(object);
		break;
	case SLIDER:
		update_slider(object);
		break;
	case WINTEXT:
		update_text(object);
		break;
	case NUMBER:
		update_number(object);
		break;
	case INPUT:
		update_input(object);
		break;
	case IMAGE:
		update_image(object);
		break;
	case BROWSER:
		update_browser(object);
		break;
	case PULLDOWN:
		update_text(object);
		break;
	case ITEM:
		update_item(object);
		break;
	case CHOICE:
		update_choice(object);
		break;
	case LISTBOX:
		update_listbox(object);
		break;
	case LISTENTRY:
		update_listentry(object);
		break;
	case MOVIE:
//GUI CHECK
		//update_movie(object);
		break;
	case ICONLIST:
		update_iconlist(object);
	case LISTENTRY_ICON:
		update_listentry_icon(object);
	}		
}


void update_objects(GuiWindow * win)
{
	GuiObject *object;

	check_window(win, "update_objects");
	object = win->first;

	while (object != NULL) {	/* update all objects in window image */
		update_object(object);
		object = object->next;
	}
}


void show_object(GuiObject * object)
{
	check_object(object, "object", "show_object");

	switch (object->objclass) {
	case BUTTON:
		show_button(object);
		break;
	case SLIDER:
		show_slider(object);
		break;
	case WINTEXT:
		show_text(object);
		break;
	case NUMBER:
		show_number(object);
		break;
	case IMAGE:
		show_image(object);
		break;
	case LISTENTRY:
		show_listentry(object);
		break;
	case LISTENTRY_ICON:
		show_listentry_icon(object);
		break;
	case MOVIE:
//GUI CHECK
		//show_movie(object);
		break;
	}		
}


void reposition_object(GuiObject * obj, int rel_x, int rel_y)
{
	check_object(obj, "obj", "reposition_object");

	obj->x += rel_x;
	obj->y += rel_y;
	obj->x_min += rel_x;
	obj->x_max += rel_x;
	obj->y_min += rel_y;
	obj->y_max += rel_y;
}


void set_object_active(GuiObject * obj, int active)
{
	check_object(obj, "obj", "set_object_active");

	obj->active = active;
}


void set_object_pressed(GuiObject * obj, int pressed)
{
	check_object(obj, "obj", "set_object_pressed");

	obj->pressed = pressed;
}


void set_object_color(GuiObject * object, int bg1, int bg2, int fg)
{
	check_object(object, "object", "set_object_color");

	object->bg_col1 = bg1;
	object->bg_col2 = bg2;
	object->fg_col = fg;
	
	create_object(object);
}


void create_object(GuiObject * object)
{
	check_object(object, "object", "create_object");

	switch (object->objclass) {
	case BUTTON:
		create_button(object);
		break;
	case SLIDER:
		create_slider(object);
		break;
	case WINTEXT:
		create_text(object);
		break;
	case NUMBER:
		create_number(object);
		break;
	case INPUT:
		create_input(object);
		break;
	case PULLDOWN:
		create_text(object);
		break;
	case ITEM:
		create_text(object);
		break;
	case LISTENTRY:
		create_text(object);
		break;
	case LISTENTRY_ICON:
		create_text(object);
		break;
	}
}


void set_object_callback(GuiObject * obj,
	       void (*function) (struct GuiObject_ * obj, int parameter))
{
	check_object(obj, "obj", "set_object_callback");

	obj->object_callback = function;
}


void set_object_callback2(GuiObject * obj,
	       void (*function) (struct GuiObject_ * obj, int parameter))
{
	check_object(obj, "obj", "set_object_callback2");

	obj->object_callback2 = function;
}


void set_object_init(GuiObject * obj,
	       void (*function) (struct GuiObject_ * obj))
{
	check_object(obj, "obj", "set_object_init");

	obj->object_init = function;
}


void set_object_info(GuiObject * obj, char *s)
{
	check_object(obj, "obj", "set_object_info");
	
	sprintf(obj->info, "%s", s);
}


void set_object_user_data(GuiObject * obj, int data)
{
	check_object(obj, "obj", "set_object_user_data");

	obj->u_data = data;
}


void set_object_align(GuiObject * obj, int align)
{
	check_object(obj, "obj", "set_object_align");

	obj->align = align;
}


void set_object_image_data(GuiObject * obj, char **data, int x_off, int y_off, int pressed, int bg_col)
{
	char buffer[80], character[20];
	int color[20], num_colors, i, count, width, height;
	int x, y, position;
	char *color_string[16] =
	{"white", "black", "darkgrey", "grey", "lightgrey",
	 "darkgreen", "green", "darkred", "red",
	 "darkblue", "blue", "lightblue", "darkyellow", "yellow",
	 "none", "special"};
	int color_int[16] =
	{(int)WHITE, (int)BLACK, (int)DARKGREY, (int)GREY, (int)LIGHTGREY,
		(int)DARKGREEN, (int)GREEN, (int)DARKRED, (int)RED,
		(int)DARKBLUE, (int)BLUE, (int)LIGHTBLUE, (int)DARKYELLOW, (int)YELLOW,
		(int)bg_col, (int)(obj->bg_col2)};

	check_object(obj, "obj", "set_object_image_data");

	if (!pressed)		/* reset special color -> LIGHT BUTTON */
		color_int[15] = DARKGREY;

	sscanf(data[0], "%d %d %d %*s ", &width, &height, &num_colors);
		
	for (count = 0; count < num_colors; count++) {
		sscanf(data[count + 1], "%c  %*cc %s\n", &character[count], buffer);
		for (i = 0; i < 16; i++)
			if (strcmp(buffer, color_string[i]) == 0) {
				color[count] = color_int[i];
			}
	}
	y = 0;
	do {
		x = 0;
		do {
			position = x + x_off + (y + y_off) * obj->width;
			for (i = 0; i < num_colors; i++)
				if (data[y + num_colors + 1][x] == character[i])
					obj->data[pressed][position] = color[i];
			x++;
		}
		while (x < width && x + x_off < obj->width);
		y++;
	}
	while (y < height && y + y_off < obj->height);
}

void set_object_image_data2(GuiObject * obj, char **data, int x_off, int y_off, int pressed, int bg_col, int width, int height, int num_colors, int *character, int *color)
{
	int x, y, position, i;
	int color_int[16] =
	{ (int)WHITE, (int)BLACK, (int)DARKGREY, (int)GREY, (int)LIGHTGREY,
		(int)DARKGREEN, (int)GREEN, (int)DARKRED, (int)RED,
		(int)DARKBLUE, (int)BLUE, (int)LIGHTBLUE, (int)DARKYELLOW, (int)YELLOW,
		(int)bg_col, (int)(obj->bg_col2)};

	check_object(obj, "obj", "set_object_image_data");

	if (!pressed)		/* reset special color -> LIGHT BUTTON */
		color_int[15] = DARKGREY;
	
	y = 0;
	do {
		x = 0;
		do {
			position = x + x_off + (y + y_off) * obj->width;
			for (i = 0; i < num_colors; i++)
				if (data[y + num_colors + 1][x] == character[i]) {
						if (color[i] == 31337) color[i]=bg_col;
						obj->data[pressed][position] = color[i];
					}
			x++;
		}
		while (x < width && x + x_off < obj->width);
		y++;
	}
	while (y < height && y + y_off < obj->height);
}


int get_object_align(GuiObject * obj, int width)
{
	int x_off = 0;
	
	check_object(obj, "obj", "get_object_align");

	switch (obj->align) {
	case ALIGN_LEFT:
		x_off = 2;
		break;
	case ALIGN_RIGHT:
		x_off = obj->width - width - 2;
		if (x_off < 2)
			x_off = 2;
		break;
	case ALIGN_CENTER:
		x_off = (obj->width - width) / 2 + 1;
		if (x_off < 1)
			x_off = 1;
		break;
	}

	return x_off;
}


void set_object_image(GuiObject * obj, char **data)
{
	int x_off, y_off;
	int width, height, num_colors;
	
	check_object(obj, "obj", "set_object_image");

	sscanf(data[0], "%d %d %d %*s\n", &width, &height, &num_colors);

	x_off = get_object_align(obj, width);
	y_off = (obj->height - height) / 2;
	if (y_off < 1)
		y_off = 1;

	/* unpressed image */
	set_object_image_data(obj, data, x_off++, y_off++, FALSE, obj->bg_col1);
	/* pressed image */
	set_object_image_data(obj, data, x_off, y_off, TRUE, obj->bg_col1);
}


void check_double_click(GuiObject *obj)
{
	GuiWinThread *win_thread;
	int count;

	if (check_object(obj, "obj", "check_double_click"))
		return;
	win_thread = (obj->win)->win_thread;
	
	count = 0;
	while (GuiMouseGetButton() != GuiMouseLeftButton && count < 30) {
		ksleep(sleep_time);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent)
			move_mouse();
		count++;
	}
	if (count < 30) {		/* Wait for the mousebutton to be released */
		while (GuiMouseGetButton() == GuiMouseLeftButton) {
			ksleep(sleep_time);
			do_window_functions(win_thread);
			if (GuiGetMessage() == GuiMouseEvent)
				move_mouse();
		}
		obj->object_callback2(obj, obj->u_data);
	}
}
