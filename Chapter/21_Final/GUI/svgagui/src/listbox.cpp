/*************************************************************************
	listbox.c -- last change: 6-9-1998

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

#define indent 5


static void listbox_slider_cb(GuiObject *slid, int data)
{
	GuiObject *listbox;
	int old_pos;

	check_object(slid, "slid", "listbox_slider_cb");
	listbox = slid->obj_link;
	check_object(listbox, "listbox", "listbox_slider_cb");
	old_pos = listbox->item_pos;
	check_window(listbox->win, "listbox_slider_cb");

	listbox->item_pos = (int)((1 - (slid->length + slid->position) / (float)slid->slider_length) *
		listbox->tot_nr_items + 0.5);

	if (old_pos != listbox->item_pos) {
		update_listbox(listbox);
		show_window(listbox->win);
	}
}


void create_listbox(GuiObject * listbox)
{
	GuiWindow *win;
	GuiObject *obj = NULL;
	GuiObject *slid, *but1, *but2;
	int width, x, y;

	check_object(listbox, "listbox", "create_listbox");
	win = listbox->win;
	check_window(win, "create_listbox");
	slid = listbox->obj_link;
	check_object(slid, "slid", "create_listbox");
	but1 = slid->prev;
	check_object(but1, "but1", "create_listbox");
	but2 = but1->prev;
	check_object(but2, "but2", "create_listbox");
	width = listbox->width - 15;

	listbox->item_pos = 0;

	for (obj = win->first; obj != NULL; obj = obj->next)
		if (obj->objclass == LISTENTRY && obj->obj_link == listbox) {
			free(obj->data[0]);
			obj->width = width - 2 * indent;
			obj->data[0] = (char *)malloc(obj->width * obj->height);
			if (obj->data[0] == NULL)
				error("Unable to allocate data array in create_listbox().");
			create_text(obj);
		}	

	x = listbox->x + width + 2 - slid->x;
	y = listbox->y + 13 - slid->y;
	reposition_object(slid, x, y);
	reposition_object(but1, x, y);
	reposition_object(but2, x, y);
	
	if (listbox->tot_nr_items < listbox->nr_items)
	{
		slid->length = slid->slider_length;
	}
	else
	{		
		//slid->length = (int)(slid->slider_length * listbox->nr_items / (float)listbox->tot_nr_items);
		slid->length = (int)(slid->slider_length * listbox->nr_items / listbox->tot_nr_items);
	}
	slid->position = slid->slider_length - slid->length;



	/* place all items in the box */
	update_listbox(listbox);
}


GuiObject *add_listbox(GuiWindow * win, int x, int y, int width, int nr_items)
{
	GuiObject *obj, *slid;

	check_window(win, "add_listbox");

	obj = (GuiObject *)malloc(sizeof(GuiObject));
	if (obj == NULL)
		error("Cannot allocate memory in add_listbox().");

	if (nr_items < 4)
		nr_items = 4;
	obj->win = win;
	obj->x = obj->x_min = x;
	obj->y = obj->y_min = y;
	obj->width = width + 15;
	obj->height = 14 * nr_items + 4;
	obj->x_max = x + obj->width - 1;
	obj->y_max = y + obj->height - 1;
	obj->nr_items = nr_items;
	obj->item_pos = 0;
	obj->tot_nr_items = 0;

	obj->align = ALIGN_LEFT;
	obj->active = TRUE;
	obj->pressed = FALSE;
	obj->hide = FALSE;
	obj->objclass = LISTBOX;
	obj->type = NORMAL_TEXT;

	obj->bg_col1 = LISTBOX_BACK;	/* background */
	obj->fg_col = LISTBOX_FORE;	/* text color */
	obj->info[0] = '\0';	/* make string length zero */
	obj->label[0] = '\0';

	obj->data[0] = NULL;
	obj->data[1] = NULL;

	add_object(obj);

	slid = add_slider(win, VERT_SLIDER, obj->x, obj->y + 13, obj->height - 26, TRUE);
	set_object_callback(slid, listbox_slider_cb);
	slid->obj_link = obj;
	obj->obj_link = slid;

	return obj;
}


void update_listbox(GuiObject * listbox)
{
	GuiWindow *win;
	GuiObject *obj, *slid;
	int count, pos;

	check_object(listbox, "listbox", "update_listbox");
	win = listbox->win;
	check_window(win, "update_listbox");
	slid = listbox->obj_link;
	check_object(slid, "slid", "update_listbox");

	win_fillbox(listbox->win, listbox->x, listbox->y,
		listbox->width - 14, listbox->height, LISTBOX_BACK);
	win_3dbox(win, DOWN_FRAME, listbox->x - 2, listbox->y - 2,
		listbox->width + 4, listbox->height + 4);

	count = 0;
	for (obj = win->first; obj != NULL; obj = obj->next)
		if (obj->objclass == LISTENTRY && obj->obj_link == listbox) {
			pos = count - listbox->item_pos;
			if (pos >= 0 && pos < listbox->nr_items) {
				obj->x_min = obj->x = listbox->x + indent;
				obj->x_max = obj->x + obj->width - 1;
				obj->y_min = obj->y = listbox->y + pos * 14 + 2;
				obj->y_max = obj->y + obj->height - 1;
				hide_object(obj, FALSE);
				update_listentry(obj);
			}
			else
				hide_object(obj, TRUE);
			count++;
		}
	update_object(slid);
}


GuiObject *add_listentry(GuiObject *listbox, char *label)
{
	GuiWindow *win;
	GuiObject *obj;
	int width;

	check_object(listbox, "listbox", "add_listentry");
	win = listbox->win;
	check_window(win, "add_listentry");

	obj = add_text(win, NORMAL_TEXT, 0, 0, label);

	obj->objclass = LISTENTRY;
	obj->bg_col1 = LISTBOX_BACK;	/* background */
	obj->fg_col = LISTBOX_FORE;	/* text color */
	create_text(obj);

	set_object_callback(obj, object_cb);
	set_object_callback2(obj, object_cb);
	obj->obj_link = listbox;

	listbox->tot_nr_items++;
	width = obj->width + 2 * indent + 15;	/* rescale the listbox automatically */
/*	if (width > listbox->width)
		listbox->width = width; */

	return obj;
}


void update_listentry(GuiObject * obj)
{
	check_object(obj, "obj", "update_listentry");

	if (!obj->hide)
		win_pixmap(obj, obj->x, obj->y, 0, obj->active);
}


void show_listentry(GuiObject * obj)
{
	check_object(obj, "obj", "show_listentry");
	check_window(obj->win, "show_listentry");

	obj->pressed = obj->pressed ? FALSE : TRUE;

	if (!obj->pressed)
		set_object_color(obj, LISTBOX_BACK, BLACK, LISTBOX_FORE);
	else
		set_object_color(obj, ACTIVE_TITLE_BACK, WIN_BACK, ACTIVE_TITLE_FORE);

	update_listentry(obj);
	show_window(obj->win);
}


void press_listbox(GuiObject * list_item)
{
	GuiWindow *win;
	GuiWinThread *win_thread;
	GuiObject *obj, *listbox;

	check_object(list_item, "list_item", "press_listbox");
	win = list_item->win;
	check_window(win, "press_listbox");
	win_thread = win->win_thread;
	listbox = list_item->obj_link;
	check_object(listbox, "listbox", "press_listbox");

	for (obj = win->first; obj != NULL; obj = obj->next)
		if (obj->objclass == LISTENTRY && obj->obj_link == listbox)
			if (obj->pressed && obj != list_item)
				show_listentry(obj);
	if (!list_item->pressed)
		show_listentry(list_item);
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
