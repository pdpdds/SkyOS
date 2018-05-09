/*************************************************************************
	number.c -- last change: 20-10-1998

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
#include "bigfont.h"
#include "local.h"
#include "windef.h"
#include "PIT.h"
#include "memory.h"

GuiObject *add_number(GuiWindow * win, int type, int x, int y, int seg_length, char *mask)
{
	GuiObject *number;
	
	check_window(win, "add_number");

	number = (GuiObject *) malloc(sizeof(GuiObject));
	if (number == NULL)
		error("Cannot allocate memory for number.");

	number->win = win;
	number->x = number->x_min = x;
	number->y = number->y_min = y;
	number->objclass = NUMBER;
	number->active = TRUE;
	number->pressed = FALSE;
	number->hide = FALSE;
	number->type = type;
	number->fg_col = NUMBER_FORE;
	number->bg_col1 = BLACK;

	number->width = 0;
	for (size_t i  = 0;i < strlen(mask);i++)
		if (mask[i] == ' ')
			number->width += 13;
		else
			number->width += bigfont_width[mask[i] - ' '];
	number->height = bigfont_height;

	number->x_max = x + number->width - 1;
	number->y_max = y + number->height - 1;
	number->align = ALIGN_LEFT;
	sprintf(number->label, "%s", mask);
	number->info[0] = '\0';	/* make string length zero */

	number->data[0] = (char *) malloc(number->width * number->height);
	if (number->data[0] == NULL)
		error("Cannot allocate memory for number.");
	number->data[1] = NULL;	/* don't use second data-block */

	create_number(number);
	add_object(number);

	return number;
}


void create_number(GuiObject * obj)
{
	check_object(obj, "obj", "create_number");

	memset(obj->data[0], obj->bg_col1, obj->width * obj->height);
	string_to_data(obj->label, 0, 0, obj->width, obj->height,
			obj->fg_col, obj->data[0], bigfont_width, bigfont_height, bigfont);
	win_object(obj);
}


void update_number(GuiObject * obj)
{
	check_object(obj, "obj", "update_number");

	create_number(obj);
}


void show_number(GuiObject * obj)
{
	check_object(obj, "obj", "show_number");
	check_window(obj->win, "show_number");

	update_number(obj);
	show_window(obj->win);
}
