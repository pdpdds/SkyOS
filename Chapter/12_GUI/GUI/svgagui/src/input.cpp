/*************************************************************************
	input.c -- last change: 20-1-1998

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
#include "string.h"
#include "ctype.h"
#include "sprintf.h"
#include "memory.h"
//#include <unistd.h>
#include "local.h"
#include "windef.h"
#include "PIT.h"


void create_input(GuiObject * input)
{
	int color, pos = string_length(input->label) + 2, i;
	int y_off, focus = FALSE;

	check_object(input, "input", "create_input");
	check_window(input->win, "create_input");

	if ((input->win)->inputfocus == input)
		focus = TRUE;
	color = focus ? input->bg_col2 : input->bg_col1;
	memset(input->data[0], color, input->width * input->height);
	y_off = (input->height - 10) / 2 + 1;
	if (y_off < 1)
		y_off = 1;
	string_to_object(input, 2, y_off, 0, NORMAL_TEXT);

	/* create cursor */
	if (focus)
		for (i = 1; i < input->height - 1; i++)
			input->data[0][pos + i * input->width] = BLACK;
}


GuiObject *add_input(GuiWindow * win, int type, int x, int y, int width, int nr_char)
{
	GuiObject *input;

	check_window(win, "add_input");

	input = (GuiObject *) malloc(sizeof(GuiObject));
	if (input == NULL)
		error("Cannot allocate memory for input.");

	input->win = win;
	input->x = input->x_min = x;
	input->y = input->y_min = y;
	input->width = width + 4;
	input->height = 15;
	input->x_max = x + input->width - 1;
	input->y_max = y + input->height - 1;
	input->align = ALIGN_LEFT;
	input->active = TRUE;
	input->pressed = FALSE;
	input->hide = FALSE;
	input->objclass = INPUT;
	input->type = type;
	input->bg_col1 = INPUT_BACK;	/* background */
	input->bg_col2 = ACTIVE_INPUT_BACK;	/* selected color */
	input->fg_col = INPUT_FORE;	/* text color */
	input->info[0] = '\0';	/* make string length zero */
	input->label[0] = '\0';
	input->length = nr_char;

	input->data[0] = (char *) malloc(input->width * input->height);
	if (input->data[0] == NULL)
		error("Unable to allocate data array in add_input().");
	input->data[1] = NULL;

	create_input(input);
	add_object(input);
	set_object_callback(input, object_cb);

	return input;
}


void update_input(GuiObject * input)
{
	check_object(input, "input", "update_input");
	check_window(input->win, "update_input");

	win_object(input);
	win_3dbox(input->win, DOWN_FRAME, input->x - 2, input->y - 2,
		  input->width + 4, input->height + 4);
}


void show_input(GuiObject * input)
{
	check_object(input, "input", "show_input");
	check_window(input->win, "show_input");

	update_input(input);
	show_window(input->win);
}


void set_input(GuiObject * input, char *s)
{
	check_object(input, "input", "set_input");

	sprintf(input->label, "%s", s);
	create_input(input);
}


void change_input(GuiObject * input, char ch)
{
	char hulp[2];
	char* label2 = nullptr;
	int size = strlen(input->label) + 1;

	if(size > 0)		
		label2 = new char[size];
	
	check_object(input, "input", "change_input");

	hulp[0] = ch;
	hulp[1] = '\0';
	
	if (ch != 127 && ch != 8) {	/* backspace or delete */
		if (strlen(input->label) < (size_t)(input->length - 1))
		if (string_length(input->label) + string_length(hulp) < input->width - 2) {
			if (((isdigit(ch) || ch == '-') && input->type == INT_INPUT) ||
			    ((isdigit(ch) || ch == '.' || ch == ',' || ch == '-' || ch == 'e' || ch == 'E') &&
			     input->type == FLOAT_INPUT) ||
			    (input->type == NORMAL_INPUT))
			    
			    
			    strcpy(label2, input->label);
				sprintf(input->label, "%s%c", label2, ch);
		}
	} else if (strlen(input->label) > 0)
		input->label[strlen(input->label) - 1] = '\0';

	create_input(input);
	show_input(input);
}


void press_input(GuiObject * input)
{
	GuiWindow *win;
	GuiObject *old;

	check_object(input, "input", "press_input");
	win = input->win;
	check_window(win, "press_input");
	old = win->inputfocus;

	if (win->inputfocus != input) {
		win->inputfocus = input;
		create_input(input);
		show_input(input);
		if (old != NULL) {
			create_input(old);
			show_input(old);
		}
	}
}
