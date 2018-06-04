/*************************************************************************
	iconlist.c -- last change: 6-9-1998

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

#include "stdint.h"
#include "windef.h"
#include "stdio.h"
#include "SkyAPI.h"
#include "local.h"
#include "images/folder.xpm"
#include "PIT.h"

#define indent 5

static void iconlist_slider_cb(GuiObject *slid, int data)
{
	GuiObject *iconlist;
	int old_pos;

	check_object(slid, "slid", "iconlist_slider_cb");
	iconlist = slid->obj_link;
	check_object(iconlist, "iconlist", "iconlist_slider_cb");
	old_pos = iconlist->item_pos;
	check_window(iconlist->win, "iconlist_slider_cb");
	
	iconlist->item_pos = (int)((1 - (slid->length + slid->position) / (float)slid->slider_length) *
			    iconlist->tot_nr_items + 0.5);

	if (old_pos != iconlist->item_pos) {
		update_iconlist(iconlist);
		show_window(iconlist->win);
	}
}


void create_iconlist(GuiObject * iconlist)
{
	GuiWindow *win;
	GuiObject *obj = NULL;
	GuiObject *slid, *but1, *but2;
	int width, x, y;
	
	check_object(iconlist, "iconlist", "create_iconlist");
	win = iconlist->win;
	check_window(win, "create_iconlist");
	slid = iconlist->obj_link;
	check_object(slid, "slid", "create_iconlist");
	but1 = slid->prev;
	check_object(but1, "but1", "create_iconlist");
	but2 = but1->prev;
	check_object(but2, "but2", "create_iconlist");
	width = iconlist->width - 15;
		
	iconlist->item_pos = 0;

	for (obj = win->first; obj != NULL; obj = obj->next)
		if (obj->objclass == LISTENTRY_ICON && obj->obj_link == iconlist) {
			free(obj->data[0]);
			obj->width = 150 - 2 * indent;
			obj->data[0] = (char *) malloc(obj->width * obj->height);
			if (obj->data[0] == NULL)
				error("Unable to allocate data array in create_iconlist().");
			create_text(obj);
		}

	x = iconlist->x + width + 2 - slid->x;
	y = iconlist->y + 13 - slid->y;
	reposition_object(slid, x, y);
	reposition_object(but1, x, y);
	reposition_object(but2, x, y);

	if (iconlist->tot_nr_items < iconlist->nr_items)
		slid->length = slid->slider_length;
	else
		slid->length = (int)(slid->slider_length * iconlist->nr_items / (float)iconlist->tot_nr_items);
	slid->position = slid->slider_length - slid->length;

	/* place all items in the box */	
	update_iconlist(iconlist);
}

GuiObject *add_iconlist(GuiWindow * win, int x, int y, int width, int nr_items)
{
	GuiObject *obj, *slid;
	

	check_window(win, "add_iconlist");

	obj = (GuiObject *) malloc(sizeof(GuiObject));
	if (obj == NULL)
		error("Cannot allocate memory in add_iconlist().");

	if (nr_items < 4)
		nr_items = 4;
	obj->win = win;
	obj->x = obj->x_min = x;
	obj->y = obj->y_min = y;
	obj->width = width + 15;
	obj->height = 40 * nr_items + 4;
	
	obj->x_max = x + obj->width - 1;
	obj->y_max = y + obj->height - 1;
	obj->nr_items = nr_items;
	obj->item_pos = 0;
	obj->tot_nr_items = 0;
	
	obj->align = ALIGN_LEFT;
	obj->active = TRUE;
	obj->pressed = FALSE;
	obj->hide = FALSE;
	obj->objclass = ICONLIST;
	obj->type = NORMAL_TEXT;

	obj->bg_col1 = LISTBOX_BACK;	/* background */
	obj->fg_col = LISTBOX_FORE;	/* text color */
	obj->info[0] = '\0';	/* make string length zero */
	obj->label[0] = '\0';
	
	obj->data[0] = NULL;
	obj->data[1] = NULL;

	add_object(obj);

	slid = add_slider(win, VERT_SLIDER, obj->x, obj->y + 13, obj->height - 26, TRUE);
	set_object_callback(slid, iconlist_slider_cb);
	slid->obj_link = obj;
	obj->obj_link = slid;

	return obj;
}


void update_iconlist(GuiObject * iconlist)
{
	GuiWindow *win;
	GuiObject *obj, *slid;
	int count, pos, v,w;

	check_object(iconlist, "iconlist", "update_iconlist");
	win = iconlist->win;
	check_window(win, "update_iconlist");
	slid = iconlist->obj_link;
	check_object(slid, "slid", "update_iconlist");

	win_fillbox(iconlist->win, iconlist->x, iconlist->y, 
		    iconlist->width - 14, iconlist->height, LISTBOX_BACK);
	win_3dbox(win, DOWN_FRAME, iconlist->x - 2, iconlist->y - 2,
		  iconlist->width + 4, iconlist->height + 4);
		  
	count = 0;
	v=0;
	w=0;
	for (obj = win->first; obj != NULL; obj = obj->next)
		if (obj->objclass == LISTENTRY_ICON && obj->obj_link == iconlist) {
			pos = count - iconlist->item_pos;
			if (pos >= 0 && pos < iconlist->nr_items*4) {

				obj->x_min = obj->x = iconlist->x + indent + v++*150;
				obj->x_max = obj->x + 80 - 1;
				obj->y_min = obj->y = iconlist->y + w * 80 + 2;
				obj->y_max = obj->y + 80 - 1;
				
				hide_object(obj, FALSE);
				update_listentry_icon(obj);
				
				if (v % 4 == 0) {
					v=0;
					w++;
				}
			} else
				hide_object(obj, TRUE);
			count++;
		}
	update_object(slid);
}


GuiObject *add_listentry_icon(GuiObject *iconlist, char *label)
{
	GuiWindow *win;
	GuiObject *obj;
	
	check_object(iconlist, "iconlist", "add_listentry_icon");
	win = iconlist->win;
	check_window(win, "add_listentry_icon");

//	obj = add_text(win, NORMAL_TEXT, 0, 0, label);
	obj = add_text_icon(win, NORMAL_TEXT, 50, 20, label, folder_xpm, 20, 20, 80);
	
	obj->objclass = LISTENTRY_ICON;
	obj->bg_col1 = LISTBOX_BACK;	/* background */
	obj->fg_col = LISTBOX_FORE;	/* text color */
		
	create_text(obj);
	
	set_object_callback(obj, object_cb);
	set_object_callback2(obj, object_cb);
	obj->obj_link = iconlist;

	iconlist->tot_nr_items++;
/*	width = obj->width + 2 * indent + 15;
	if (width > iconlist->width)
		iconlist->width = width; */
	
	return obj;
}


void update_listentry_icon(GuiObject * obj)
{
	check_object(obj, "obj", "update_listentry_icon");
	int folder_character[4] = {' ','.','+','@'};
	int folder_color[4] = {31337, (int)BLACK, (int)RED, (int)WHITE};
	
	if (!obj->hide) {
		set_object_image_data2(obj, folder_xpm, 30, 0, FALSE, obj->bg_col1, 48, 48, 4, folder_character, folder_color);
		win_pixmap(obj, obj->x, obj->y, 0, obj->active);
	}
}


void show_listentry_icon(GuiObject * obj)
{
	check_object(obj, "obj", "show_listentry_icon");
	check_window(obj->win, "show_listentry_icon");

	obj->pressed = obj->pressed ? FALSE : TRUE;
	
	if (!obj->pressed)
		set_object_color(obj, LISTBOX_BACK, BLACK, LISTBOX_FORE);
	else
		set_object_color(obj, ACTIVE_TITLE_BACK, WIN_BACK, ACTIVE_TITLE_FORE);
	
	update_listentry_icon(obj);
	show_window(obj->win);
}


void press_iconlist(GuiObject * list_item)
{
	GuiWindow *win;
	GuiWinThread *win_thread;
	GuiObject *obj, *iconlist;
	
	check_object(list_item, "list_item", "press_iconlist");
	win = list_item->win;
	check_window(win, "press_iconlist");
	win_thread = win->win_thread;
	iconlist = list_item->obj_link;
	check_object(iconlist, "iconlist", "press_iconlist");

	for (obj = win->first; obj != NULL; obj = obj->next)
		if (obj->objclass == LISTENTRY_ICON && obj->obj_link == iconlist)
			if (obj->pressed && obj != list_item)
				show_listentry_icon(obj);
	if (!list_item->pressed)
		show_listentry_icon(list_item);
	list_item->object_callback(list_item, list_item->u_data);

	/* Wait for the mousebutton to be released */
	while (GuiMouseGetButton() == GuiMouseLeftButton) {
		ksleep(sleep_time);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent)
			move_mouse();
	}
	check_double_click(list_item);
}
