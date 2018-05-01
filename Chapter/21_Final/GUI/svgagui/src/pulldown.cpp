/*************************************************************************
	pulldown.c -- last change: 6-9-1998

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
#include "string.h"
#include "memory.h"
#include "sprintf.h"
#include "local.h"
#include "windef.h"
#include "PIT.h"
#include "images/check_black.xpm"
#include "images/check_grey.xpm"
#include "images/right.xpm"
#include "images/right_grey.xpm"

#define text_indent 15
#define line_height 8

static void dummy_init(GuiObject * obj)
{
}


GuiObject *add_item(GuiObject * popup, char *label, int type)
{
	GuiWindow *win = popup->win_link;
	GuiObject *obj;

	check_object(popup, "popup", "add_item");

	obj = (GuiObject *) malloc(sizeof(GuiObject));
	if (obj == NULL)
		error("Cannot allocate memory for button.");

	obj->win = win;
	obj->win_link = NULL;
	obj->pressed = FALSE;
	obj->active = TRUE;
	obj->hide = FALSE;
	obj->objclass = ITEM;
	obj->type = type;
	obj->fg_col = TEXT_NORMAL;	/* foreground */
	obj->bg_col1 = win->bg_col;	/* background not highlighted */
	obj->bg_col2 = TEXT_HIGHLIGHT;	/* background highlighted */
	obj->u_data = 0;
	sprintf(obj->label, "%s", label);
	obj->align = ALIGN_LEFT;
	obj->info[0] = '\0';	/* make string length zero */
	set_object_callback(obj, object_cb);
	set_object_init(obj, dummy_init);

	add_object(obj);
	win->number++;
	
	obj->data[0] = NULL;
	obj->data[1] = NULL;
	if (type == POPUP_ITEM) {
		/* create a new window that will contain the pulldown menu */
		obj->win_link = add_window(win->win_thread, NO_TITLE_WINDOW, win->x, win->y, 1, 1, "", FALSE, TRUE);
		/* memory allocation will be performed in create_pulldown() */
		obj->sep_count = 0;
	}
	return obj;	
}


void add_pd_separator(GuiObject * pulldown)
{
	GuiWindow *win;
	
	check_object(pulldown, "pulldown", "add_pd_separator");
	win = pulldown->win_link;
	check_window(win, "add_pd_separator");
	
	if (pulldown->sep_count < 10) {
		pulldown->sep[pulldown->sep_count] = win->number;
		pulldown->sep_count++;	
	}
}


void create_pulldown(GuiObject * pulldown)
{
	GuiWindow *win;
	GuiObject *obj;
	int width = 0, height = 0, w, pos, count = 0, s_count = 0;
	
	check_object(pulldown, "pulldown", "create_pulldown");
	win = pulldown->win_link;
	check_window(win, "create_pulldown");
	
	for (obj = win->first; obj != NULL; obj = obj->next) {
		w = string_length(obj->label);
		if (w > width)
			width = w;
		height += 14;
	}
	width += 2 * text_indent + 9;
	height += 6 + line_height * pulldown->sep_count;
	
	win->width = width;
	win->height = height;
	win->hide = TRUE;
	set_maximum_update_region(win);
	free(win->data);
	win->data = (char *)malloc(width * height);
	if (win->data == NULL)
		error("Cannot allocate memory in create_pulldown().");

	/* Allocate memory for the items */
	for (obj = win->first; obj != NULL; obj = obj->next) {
		obj->x = 2;
		obj->y = 3;
		obj->width = width - 5;
		obj->height = 14;

		obj->data[0] = (char *)malloc(obj->width * obj->height);
		if (obj->data[0] == NULL)
			error("Cannot allocate memory in create_pulldown().");
	}	
	create_window(win);

	/* create all the items */	
	pos = 3;
	for (obj = win->first; obj != NULL; obj = obj->next) {
		obj->x_min = obj->x;
		obj->y_min = obj->y = pos;
		obj->x_max = obj->x + obj->width - 1;
		obj->y_max = obj->y + obj->height - 1;

		memset(obj->data[0], obj->bg_col1, obj->width * obj->height);
		if (obj->active)
			string_to_object(obj, text_indent, 2, 0, NORMAL_TEXT);
		else
			string_to_object(obj, text_indent, 2, 0, EMBOSSED_TEXT);
		if (obj->type == CHECK_ITEM && obj->pressed)
			set_object_image_data(obj, check_black_xpm, 2, 3, FALSE, obj->bg_col1);
		if (obj->type == POPUP_ITEM) {
			set_object_image_data(obj, right_xpm, obj->width - 8, 3, FALSE, obj->bg_col1);
			if (obj->win_link != NULL) {
				(obj->win_link)->x = win->x + obj->x + obj->width;
				(obj->win_link)->y = win->y + obj->y;
			}
		}
		pos += obj->height;
		count++;
		if (pulldown->sep_count > 0 && pulldown->sep[s_count] == count) {
			pos += (line_height - 2) / 2;
			win_3dline(win, 2, pos, width - 2, pos);
			s_count++;
			pos += (line_height + 2) / 2;
		}
	}
	update_objects(win);
}


GuiObject *add_pulldown(GuiWindow * win, int x, int y, char *label)
{
	GuiWinThread *win_thread;
	GuiWindow *win_p;
	GuiObject *obj;
	int win_x, win_y;

	check_window(win, "add_pulldown");
	win_thread = win->win_thread;

	obj = (GuiObject *) malloc(sizeof(GuiObject));
	if (obj == NULL)
		error("Cannot allocate memory for input.");

	obj->win = win;
	obj->x = obj->x_min = x;
	obj->y = obj->y_min = y;
	obj->width = string_length(label) + 4;
	obj->height = 14;
	obj->x_max = x + obj->width - 1;
	obj->y_max = y + obj->height - 1;
	obj->align = ALIGN_LEFT;
	obj->active = TRUE;
	obj->pressed = FALSE;
	obj->hide = FALSE;
	obj->objclass = PULLDOWN;
	obj->type = NORMAL_TEXT;
	obj->bg_col1 = WIN_BACK;	/* background */
	obj->fg_col = TEXT_NORMAL;	/* text color */
	obj->object_init = dummy_init;
	sprintf(obj->label, "%s", label);
	obj->info[0] = '\0';	/* make string length zero */

	obj->data[0] = (char *) malloc(obj->width * obj->height);
	if (obj->data[0] == NULL)
		error("Unable to allocate data array in add_pulldown().");
	obj->data[1] = NULL;

	create_text(obj);
	add_object(obj);
	
	/* create a new window that will contain the pulldown menu */
	win_x = win->x + obj->x;
	win_y = win->y + obj->y + obj->height;
	win_p = add_window(win_thread, NO_TITLE_WINDOW, win_x, win_y, 1, 1, "", FALSE, TRUE);
	/* memory allocation will be performed in create_pulldown() */
	obj->win_link = win_p;
	obj->sep_count = 0;

	return obj;
}


void set_item_focus(GuiObject * obj, int focus, int indent)
{
	check_object(obj, "obj", "set_item_focus");
	check_window(obj->win, "set_item_focus");
	
	if (!focus) {
		obj->bg_col1 = (obj->win)->bg_col;
		obj->bg_col2 = (obj->win)->bg_col;
		obj->fg_col = TEXT_NORMAL;
	} else {
		obj->bg_col1 = ACTIVE_TITLE_BACK;
		obj->bg_col2 = WIN_BACK;
		obj->fg_col = ACTIVE_TITLE_FORE;
	}
	memset(obj->data[0], obj->bg_col1, obj->width * obj->height);
	if (obj->active)
		string_to_object(obj, indent, 2, 0, NORMAL_TEXT);
	else if (!focus)
		string_to_object(obj, indent, 2, 0, EMBOSSED_TEXT);
	else {
		obj->fg_col = DARKGREY;
		string_to_object(obj, indent, 2, 0, NORMAL_TEXT);
	}		
	if (obj->type == CHECK_ITEM && obj->pressed) {
		if (focus)
			set_object_image_data(obj, check_grey_xpm, 2, 3, FALSE, obj->bg_col1);
		else
			set_object_image_data(obj, check_black_xpm, 2, 3, FALSE, obj->bg_col1);
	}
	if (obj->type == POPUP_ITEM) {
		if (focus)
			set_object_image_data(obj, right_grey_xpm, obj->width - 8, 3, FALSE, obj->bg_col1);
		else
			set_object_image_data(obj, right_xpm, obj->width - 8, 3, FALSE, obj->bg_col1);
	}
	update_item(obj);

 	show_window(obj->win);
	if (obj->type == CHECK_ITEM && !obj->pressed)
		win_fillbox(obj->win, obj->x + 2, obj->y + 3, 8, 8, obj->bg_col1);
}


void update_item(GuiObject * obj)
{
	check_object(obj, "obj", "update_item");

	win_pixmap(obj, obj->x, obj->y, 0, TRUE);
}


void disable_item(GuiObject * obj, int disable)
{
	check_object(obj, "obj", "disable_item");

	obj->active = disable ? FALSE : TRUE;
	show_item(obj);
}


void show_item(GuiObject * obj)
{
	int indent; 
	
	check_object(obj, "obj", "show_item");
	
	if (obj->objclass == CHOICE)
		indent = 5;
	else 
		indent = 15;

	set_item_focus(obj, FALSE, indent);
}


static GuiObject *pulldown_loop(GuiObject * obj)
{
	GuiWindow *win, *win_p;
	GuiWinThread *win_thread;
	GuiObject *object = NULL, *ret_obj = NULL;
	int exit_loop = FALSE, next_pulldown = FALSE, next_item = FALSE;
	int mouse_hold = FALSE, status = 0;
	int popup = FALSE;

	check_object(obj, "obj", "pulldown_loop");
	win = obj->win_link;
	check_window(win, "pulldown_loop");
	win_p = obj->win;
	check_window(win_p, "pulldown_loop");
	win_thread = win->win_thread;

	if (obj->type == POPUP_ITEM)
		popup = TRUE;
	else {
		set_object_color(obj, ACTIVE_TITLE_BACK, WIN_BACK, ACTIVE_TITLE_FORE);
		show_text(obj);
	}
	win_thread->focuswin = win;
	obj->object_init(obj);

	if (popup) {
		win->x = win_p->x + obj->x + obj->width + 3;
		win->y = win_p->y + obj->y - 3;
	} else {
		win->x = win_p->x + obj->x;
		win->y = win_p->y + obj->y + obj->height;
	}
	unhide_window(win, TRUE);
	bring_window_to_top(win);
	
	/* wait until the user leaves the object */
	object_region(win_p);
	while (win_p->focusobj == obj) {
		ksleep(sleep_time);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent) {
			move_mouse();
			object_region(win_p);
		}
	}
	/* determine on what status to exit */
	if (GuiMouseGetButton() == GuiMouseLeftButton)
		mouse_hold = TRUE;
	if (win_p->focusobj != NULL)
		if ((win_p->focusobj)->objclass == ITEM) {
			exit_loop = TRUE;
			next_item = TRUE;
		}
	while (!exit_loop) {
		ksleep(sleep_time);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent) {
			move_mouse();
			object_region(win);
			if (win->focusobj == NULL) {	/* mouse is not in the active pulldown */
				object_region(win_p);
				if (win_p->focusobj != NULL) {
					if (win_p->focusobj != obj && (win_p->focusobj)->objclass == PULLDOWN) {
						next_pulldown = TRUE;
						exit_loop = TRUE;
					}
					if (win_p->focusobj != obj && (win_p->focusobj)->objclass == ITEM) {
						exit_loop = TRUE;
						next_item = TRUE;
					}
				}
			}
			if (win->focusobj != object) {	/* mouse moved to new item */
				if (object != NULL && object->objclass == ITEM) {
					set_item_focus(object, FALSE, text_indent);
					object = NULL;
				}
				if (win->focusobj != NULL) {
					object = win->focusobj;
					if (object->objclass == ITEM) {
						set_item_focus(object, TRUE, text_indent);
						if (object->type == POPUP_ITEM && object->active) {
							ret_obj = pulldown_loop(object);
							if (ret_obj != NULL)
								exit_loop = TRUE;
							else {
								object_region(win);
								if (win->focusobj == NULL)
									exit_loop = TRUE;
							}
						}
					}
				}
			}
			status = GuiMouseGetButton();
			if ((status == GuiMouseLeftButton && !mouse_hold) ||
			    (status != GuiMouseLeftButton && mouse_hold))
				exit_loop = TRUE;
		}
	}

	/* wait for mousebutton to be released */
	while (status == GuiMouseLeftButton && !next_pulldown && !next_item) {
		ksleep(sleep_time);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent)
			status = GuiMouseGetButton();
	}
	
	hide_window(obj->win_link, TRUE);
	if (!popup) {
		set_object_color(obj, WIN_BACK, WIN_BACK, TEXT_NORMAL);
		show_text(obj);
	}
	if (object != NULL) {	/* reset the object focus */
		if (win->focusobj != NULL) {
			if (object->type == CHECK_ITEM)
				object->pressed = object->pressed ? FALSE : TRUE;
			if (object->active)
				ret_obj = object;
			win->focusobj = NULL;
		}
		set_item_focus(object, FALSE, text_indent);
	}
	if (next_pulldown)
		ret_obj = pulldown_loop(win_p->focusobj);

	return ret_obj;
}


void press_pulldown(GuiObject *pulldown)
{
	GuiObject *obj = NULL;
	
	check_object(pulldown, "pulldown", "press_pulldown");
	
	obj = pulldown_loop(pulldown);
	if (obj != NULL)
		obj->object_callback(obj, obj->u_data);
}
