/*************************************************************************
	browser.c -- last change: 20-1-1998

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
#include "memory.h"
#include "sprintf.h"
#include "string.h"
#include "local.h"

extern int font_width[], font_height;
extern char *font[];

static void browser_slider_cb(GuiObject * obj, int data)
{
	GuiObject *browser;
	
	check_object(obj, "obj", "browser_slider_cb");
	browser = obj->obj_link;
	check_object(browser, "browser", "browser_slider_cb");

	if (data == 0)	/* move horizontally */
		browser->b_x = obj->position * (browser->b_width / (float)obj->slider_length);
	else	/* move vertically */
		browser->b_y = (obj->slider_length - obj->position - obj->length) *
				(browser->b_height / (float)obj->slider_length);
	show_browser(browser);
}


void set_browser_text(GuiObject * obj, char *lines)
{
	GuiObject *slid;
	int x, barlength, count;
	char *line, *hulp;
	
	check_object(obj, "obj", "set_browser_text");
	
	if (strlen(lines) == 0)
		return;
	else {
		hulp = (char *) malloc(strlen(lines) + 1);
		if (hulp == NULL)
			error("Cannot allocate memory in set_browser_text().");
	}
	
	if (obj->data[0] != NULL)	/* free the block for new text */
		kfree(obj->data[0]);
	obj->data[1] = NULL;	/* don't use second data-block */

	obj->b_x = 0;
	obj->b_y = 0;
	obj->b_width = 0;
	obj->b_height = 0;

	determine_geometry(lines, &(obj->b_width), &(obj->b_height));

	if (obj->b_width < obj->width)
		obj->b_width = obj->width;
	if (obj->b_height < obj->height)
		obj->b_height = obj->height;

	if (obj->b_width == 0 || obj->b_height == 0)
		return;

	obj->data[0] = (char *) malloc(obj->b_width * obj->b_height);
	if (obj->data[0] == NULL)
		error("Unable to allocate data array in set_browser_text().");
	memset(obj->data[0], obj->bg_col1, obj->b_width * obj->b_height);

	/* copy strings into the image of the browser */
	count = 0;
	x = get_object_align(obj, obj->b_width);
	sprintf(hulp, "%s", lines);
	sprintf(obj->buffer, "%s", lines);
	line = strtok(hulp, "\n");
	while (line != NULL) {
		string_to_data(line, x, count * 13, obj->b_width, obj->b_height,
				 obj->fg_col, obj->data[0], font_width, font_height, font);
		line = strtok(NULL, "\n");
		count++;
	}
	kfree(hulp);
	
	/* Resize sliders? */
	if (!obj->sliders)
		return;
	
	slid = obj->next;	/* find horizontal slider */
	while (slid->objclass != SLIDER)
		slid = slid->next;
	barlength = (int)(obj->width / (float)obj->b_width * slid->slider_length);
	slid->length = barlength;
	set_slider_position(slid, 0);
	update_slider(slid);

	slid = slid->next;	/* find vertical slider */
	while (slid->objclass != SLIDER)
		slid = slid->next;
	barlength = (int)(obj->height / (float)obj->b_height * slid->slider_length);
	slid->length = barlength;
	set_slider_position(slid, slid->slider_length - barlength);
	update_slider(slid);
}


GuiObject *add_browser(GuiWindow * win, int x, int y, int width, int height, int sliders)
{
	GuiObject *obj, *slid;
	int length;
	
	check_window(win, "add_browser");

	obj = (GuiObject *) malloc(sizeof(GuiObject));
	if (obj == NULL)
		error("Cannot allocate memory for browser.");

	obj->win = win;
	obj->x = obj->x_min = x;
	obj->y = obj->y_min = y;
	obj->width = width;
	obj->height = height;
	obj->x_max = x + obj->width - 1;
	obj->y_max = y + obj->height - 1;
	obj->b_x = 0;
	obj->b_y = 0;

	obj->objclass = BROWSER;
	obj->hide = FALSE;
	obj->active = TRUE;
	obj->pressed = FALSE;
	obj->type = NORMAL_BROWSER;
	obj->sliders = sliders;

	obj->fg_col = BROWSER_FORE;
	obj->bg_col1 = BROWSER_BACK;
	obj->align = ALIGN_LEFT;
	obj->label[0] = '\0';
	obj->info[0] = '\0';	/* make string length zero */
	
	obj->data[0] = NULL;	/* data blocks will be initialized in set_browser_text */
	obj->data[1] = NULL;

	add_object(obj);
	
	if (sliders) {		/* add sliders */
		length = obj->width - 24;
		slid = add_slider(win, HOR_SLIDER, obj->x + 12, obj->y + obj->height + 2,
				 length, TRUE);
		set_object_callback(slid, browser_slider_cb);
		set_object_user_data(slid, 0);
		slid->obj_link = obj;

		length = obj->height - 24;
		slid = add_slider(win, VERT_SLIDER, obj->x + obj->width + 2, obj->y + 12,
				 length, TRUE);
		set_object_callback(slid, browser_slider_cb);
		set_object_user_data(slid, 1);
		slid->obj_link = obj;
	}
	return obj;
}


static void project_data_on_object(GuiObject *obj)
{
	GuiWindow *win;
	int i;
	int w, h, w_;
	char *source, *dest;

	check_object(obj, "obj", "project_data_on_object");
	win = obj->win;
	check_window(win, "project_data_on_object");
	w = obj->width;
	h = obj->height;
	w_ = win->width;

	if (obj->b_x + obj->width >= obj->b_width)
		obj->b_x = obj->b_width - obj->width;
	if (obj->b_y + obj->height >= obj->b_height)
		obj->b_y = obj->b_height - obj->height;
	
	if (obj->x + obj->width > win->width - 1)
		w = win->width - 1 - obj->x;
	if (obj->y + obj->height > win->height - 1)
		h = win->height - 1 - obj->y;
		
	dest = win->data + obj->x + obj->y * win->width;
	source = obj->data[0] + obj->b_x + obj->b_y * obj->b_width;
	for (i = h; i > 0; i--) {
		memcpy(dest, source, w);
		dest += w_;
		source += obj->b_width;
	}
	win_box(win, obj->x - 1, obj->y - 1, obj->width + 2, obj->height + 2, obj->bg_col1);
}


void update_browser(GuiObject * obj)
{
	check_object(obj, "obj", "update_browser");

	project_data_on_object(obj);
}


void show_browser(GuiObject * obj)
{
	check_object(obj, "obj", "show_browser");

	update_browser(obj);
	show_window(obj->win);
}
