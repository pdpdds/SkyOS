/*************************************************************************
	choice.c -- last change: 27-9-1998

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
#include "SkyAPI.h"
#include "string.h"
#include "memory.h"
#include "sprintf.h"
#include "windef.h"
#include "PIT.h"
#include "local.h"
#include "images/down.xpm"

#define text_indent 5


static void choice_cb(GuiObject *obj, int data)
{
	check_object(obj, "obj", "choice_cb");

	show_button(obj);
	press_choice(obj->obj_link);
}


void create_choice(GuiObject *choice)
{
	GuiWindow *win;
	GuiObject *obj;
	int pos, count = 0;

	check_object(choice, "choice", "create_choice");
	win = choice->win_link;
	check_window(win, "create_choice");

	choice->height = 14;
	choice->data[0] = (char *) malloc(choice->width * choice->height);
	if (choice->data[0] == NULL)
		error("Unable to allocate data array in create_choice().");
	choice->data[1] = NULL;

	choice->x_min = choice->x;
	choice->y_min = choice->y;
	choice->x_max = choice->x + choice->width - 1;
	choice->y_max = choice->y + choice->height - 1;
	sprintf(choice->label, "%s", (win->first)->label);
	create_text(choice);
	/* add a button to the window */
	obj = add_button(choice->win, PIXMAP_BUTTON, choice->x + choice->width, 
			choice->y, 14, 14, "");
	obj->obj_link = choice;
	set_object_image_data(obj, down_xpm, 3, 5, 0, obj->bg_col1);
	set_object_image_data(obj, down_xpm, 4, 6, 1, obj->bg_col1);
	set_object_callback(obj, choice_cb);

	win->width = choice->width + 18;
	win->height = 0;
	for (obj = win->first; obj != NULL; obj = obj->next) 
		win->height += 14;
	win->height += 5;

	set_maximum_update_region(win);
	if (win->data)
		kfree(win->data);
	win->data = (char *)malloc(win->width * win->height);
	if (win->data == NULL)
		error("Cannot allocate memory in create_choice().");
	
	/* create the window */
	win_fillbox(win, 0, 0, win->width, win->height, win->bg_col);
	win_box(win, 0, 0, win->width, win->height, BLACK);
	/* create all the items */	
	pos = 3;
	for (obj = win->first; obj != NULL; obj = obj->next) {
		obj->x = obj->x_min = 2;
		obj->y = obj->y_min = pos;
		obj->width = win->width - 4;
		obj->height = 14;
		obj->x_max = obj->x + obj->width - 1;
		obj->y_max = obj->y + obj->height - 1;
		
		obj->data[0] = (char *)malloc(obj->width * obj->height);
		if (obj->data[0] == NULL)
			error("Cannot allocate memory in create_pulldown().");
		obj->data[1] = NULL;
		memset(obj->data[0], obj->bg_col1, obj->width * obj->height);
		string_to_object(obj, text_indent, 2, 0, obj->type);
		pos += obj->height;
		count++;
	}
	update_objects(win);
}


GuiObject *add_choice(GuiWindow * win, int x, int y, int width)
{
	GuiWinThread *win_thread = win->win_thread;
	GuiWindow *win_p;
	GuiObject *obj;

	check_window(win, "add_choice");

	obj = (GuiObject *) malloc(sizeof(GuiObject));
	if (obj == NULL)
		error("Cannot allocate memory for input.");

	obj->win = win;
	obj->x = x;
	obj->y = y;
	obj->width = width;
	obj->align = ALIGN_LEFT;
	obj->active = TRUE;
	obj->pressed = FALSE;
	obj->hide = FALSE;
	obj->objclass = CHOICE;
	obj->type = NORMAL_TEXT;
	obj->bg_col1 = CHOICE_BACK;	/* background */
	obj->fg_col = TEXT_NORMAL;	/* text color */
	obj->info[0] = '\0';	/* make string length zero */

	add_object(obj);
	
	/* create a new window that will contain the choice menu */
	win_p = add_window(win_thread, NO_TITLE_WINDOW, win->x,
			   win->y, 5, 5, "", FALSE, TRUE);
	win_p->hide = TRUE;
	win_p->bg_col = obj->bg_col1;
	obj->win_link = win_p;

	/* memory allocation will be performed in create_choice() */

	return obj;
}


void update_choice(GuiObject * obj)
{
	check_object(obj, "obj", "update_choice");

	win_object(obj);
	win_3dbox(obj->win, DOWN_FRAME, obj->x - 2, obj->y - 2, 
		  obj->width + 14 + 4, obj->height + 4);
}


void press_choice(GuiObject * obj)
{
	GuiWindow *win;
	GuiWinThread *win_thread;
	GuiObject *object = NULL;
	int exit_loop = FALSE;
	int mouse_hold = FALSE, status = 0;

	check_object(obj, "obj", "press_choice");
	win = obj->win_link;
	check_window(win, "press_choice");
	win_thread = win->win_thread;

	win_thread->focuswin = win;

	(obj->win_link)->x = (obj->win)->x + obj->x - 2;
	(obj->win_link)->y = (obj->win)->y + obj->y + obj->height + 2;
	unhide_window(obj->win_link, TRUE);
	if (!win_thread->auto_raise)
		bring_window_to_top(obj->win);
	bring_window_to_top(obj->win_link);
	
	/* wait until the users leaves the pulldown title */
	object_region(obj->win);
	while ((obj->win)->focusobj == obj) {
		//ksleep(sleep_time);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent) {
			move_mouse();
			object_region(obj->win);
		}
	}
	/* determine on what status to exit */
	if (GuiMouseGetButton() == GuiMouseLeftButton)
		mouse_hold = TRUE;
	do {
		//ksleep(sleep_time);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent) {
			move_mouse();
			object_region(win);
			if (win->focusobj != object) {
				if (object != NULL && object->objclass == ITEM) {
					set_item_focus(object, FALSE, text_indent);
					object = NULL;
				}
				if (win->focusobj != NULL) {
					object = win->focusobj;
					if (object->objclass == ITEM)
						set_item_focus(object, TRUE, text_indent);
				}
			}
			status = GuiMouseGetButton();
			if ((status == GuiMouseLeftButton && !mouse_hold) ||
			    (status != GuiMouseLeftButton && mouse_hold))
				exit_loop = TRUE;
		}
	}
	while (!exit_loop);

	/* wait for mousebutton to be released */
	while (status == GuiMouseLeftButton) {
		//ksleep(sleep_time);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent) {
			move_mouse();
			status = GuiMouseGetButton();
		}
	}
	
	hide_window(obj->win_link, TRUE);

	if (object != NULL) {	/* reset the object focus */
		if (win->focusobj != NULL) {
			set_text(obj, object->label);
			show_text(obj);
			object->object_callback(object, object->u_data);
			win->focusobj = NULL;
		}
		set_item_focus(object, FALSE, text_indent);
	}
}
