/*************************************************************************
	text.c -- last change: 20-1-1998

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
#include "sprintf.h"
#include "string.h"
#include "memory.h"
#include "local.h"
#include "font.h"
#include "windef.h"
#include "PIT.h"

int string_length(char *s)
{
	int width = 0;

	for (int i = 0; i < (int)strlen(s); i++)
		width += font_width[s[i] - ' '];

	if (strlen(s) > 256) {
//		fprintf(stderr, "---> Warning: string length > 256 characters!\n");
		width = 0;
	}
	return width;
}


void string_to_data(char *label, int x_off, int y_off, int width, int height, 
		    int color, char *data, int *font_w, int font_h, char **font_data)
{
	int position, pos = 0, x, y, i;
	char ch;

	for (size_t count = 0; count < strlen(label); count++) {
		ch = label[count];
		if (ch >= ' ' && ch <= '~') {
			pos = ch - ' ';

			position = 0;	/* the position in character list */
			for (i = 0; i < pos; i++)
				position += font_w[i];
			y = 0;
			do {
				x = 0;
				while (x < font_w[pos] && x + x_off < width) {
					if (font_data[y][x + position] == 'x' && y + y_off < height)
						data[x + x_off + (y + y_off) *
						width] = color;
					x++;
				}
				y++;
			}
			while (y < font_h && y + y_off < height);
			x_off += font_w[pos];
		}
	}
}


void string_to_object(GuiObject * obj, int x_off, int y_off, int element, int type)
{
	check_object(obj, "obj", "string_to_object");

	if (type == NORMAL_TEXT)
		string_to_data(obj->label, x_off, y_off, obj->width, obj->height,
				obj->fg_col, obj->data[element], font_width, font_height, font);
	if (type == EMBOSSED_TEXT) {
		string_to_data(obj->label, x_off + 1, y_off + 1, obj->width, obj->height,
				WHITE, obj->data[element], font_width, font_height, font);
		string_to_data(obj->label, x_off, y_off, obj->width, obj->height,
				TEXT_EMBOSS, obj->data[element], font_width, font_height, font);
	}
}


void determine_geometry(char *lines, int *width, int *height)
{
	char *hulp, *line;
	int w;

	hulp = (char *)malloc(strlen(lines) + 1);
	if (hulp == NULL)
		error("Cannot allocate memory in determine_geometry().");

	*width = 0;
	*height = 0;
	sprintf(hulp, "%s", lines);
	line = strtok(hulp, "\n");
	while (line != NULL) {
		w = string_length(line);
		if (w > *width)
			*width = w;
		*height += 13;
		line = strtok(NULL, "\n");
	}
	kfree(hulp);
}


GuiObject *add_text(GuiWindow * win, int type, int x, int y, char *label)
{
	GuiObject *text;

	check_window(win, "add_text");

	text = (GuiObject *) malloc(sizeof(GuiObject));
	if (text == NULL)
		error("Cannot allocate memory for text object.");

	text->win = win;
	text->x = text->x_min = x;
	text->y = text->y_min = y;
	text->objclass = WINTEXT;
	text->hide = FALSE;
	text->active = TRUE;
	text->pressed = FALSE;
	text->type = type;
	text->fg_col = TEXT_NORMAL;
	text->bg_col1 = WIN_BACK;
	text->width = string_length(label) + 4;
	text->height = 14;
	text->x_max = x + text->width - 1;
	text->y_max = y + text->height - 1;
	text->align = ALIGN_LEFT;
	sprintf(text->label, "%s", label);
	text->info[0] = '\0';	/* make string length zero */

	text->data[0] = (char *) malloc(text->width * text->height);
	if (text->data[0] == NULL)
		error("Unable to allocate data array in add_text().");
	text->data[1] = NULL;	/* don't use second data-block */

	create_text(text);
	add_object(text);

	return text;
}

GuiObject *add_text_icon(GuiWindow * win, int type, int x, int y, char *label, char **icon, int icon_xoffs, int icon_yoffs, int heightx)
{
	GuiObject *text;

	check_window(win, "add_text");

	text = (GuiObject *) malloc(sizeof(GuiObject));
	if (text == NULL)
		error("Cannot allocate memory for text object.");

	text->win = win;
	text->x = text->x_min = x;
	text->y = text->y_min = y;
	text->objclass = WINTEXT;
	text->hide = FALSE;
	text->active = TRUE;
	text->pressed = FALSE;
	text->type = type;
	text->fg_col = TEXT_NORMAL;
	text->bg_col1 = WIN_BACK;
	text->width = string_length(label) + 4;
	text->height = heightx;
	text->x_max = x + text->width - 1;
	text->y_max = y + text->height - 1;
	text->align = ALIGN_LEFT;
	sprintf(text->label, "%s", label);
	text->info[0] = '\0';	/* make string length zero */

	text->data[0] = (char *) malloc(text->width * text->height);
	if (text->data[0] == NULL)
		error("Unable to allocate data array in add_text().");
	text->data[1] = NULL;	/* don't use second data-block */

	create_text(text);
	add_object(text);

	return text;
}

void create_text(GuiObject * obj)
{
	int x_off, y_off;
	int width;

	check_object(obj, "obj", "create_text");
	width = string_length(obj->label);

	memset(obj->data[0], obj->bg_col1, obj->width * obj->height);

	x_off = get_object_align(obj, width);
	y_off = (obj->height - 10) / 2;
	if (y_off < 1)
		y_off = 1;

	if (obj->objclass == LISTENTRY_ICON) {
		string_to_object(obj, 35, 60, 0, obj->active ? obj->type : EMBOSSED_TEXT);		
	} else {
		string_to_object(obj, x_off, y_off, 0, obj->active ? obj->type : EMBOSSED_TEXT);	
	}

}

void set_text(GuiObject * text, char *s)
{
	check_object(text, "text", "set_text");

	sprintf(text->label, "%s", s);
	create_text(text);
}


void update_text(GuiObject * text)
{
	check_object(text, "text", "update_text");
	win_object(text);
}


void show_text(GuiObject * text)
{
	check_object(text, "text", "show_text");
	check_window(text->win, "show_text");

	update_text(text);
	show_window(text->win);
}


void set_text_active(GuiObject * text, int active)
{
	check_object(text, "text", "set_text_active");

	text->active = active;
	create_text(text);
	update_text(text);
}
