/*************************************************************************
	dialog.c -- last change: 23-8-1998

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
#include "local.h"
#include "windef.h"
#include "PIT.h"
#include "images/dia_exclamation.xpm"
#include "images/dia_info.xpm"
#include "images/dia_question.xpm"
#include "images/dia_stop.xpm"


static void add_symbol(GuiWindow *win, int symbol)
{
	if (symbol == 0)
		return;
		
	check_window(win, "add_symbol");

	switch(symbol) {
		case DIA_EXCLAMATION :
			add_image(win, 10, (win->height - 32) / 2 + 5, dia_exclamation);
			break;
		case DIA_INFO :
			add_image(win, 10, (win->height - 32) / 2 + 5, dia_info);
			break;
		case DIA_QUESTION :
			add_image(win, 10, (win->height - 32) / 2 + 5, dia_question);
			break;
		case DIA_STOP :
			add_image(win, 10, (win->height - 32) / 2 + 5, dia_stop);
			break;
	}
}


int question_dialog(GuiWinThread *win_thread, GuiWindow *p_win, char *message, int symbol)
{
	GuiWindow *win;
	GuiObject *obj = NULL, *Yes, *No;
	int width, height, x, y, pos = 0, count = 0;
	char *hulp, *line;
	int result;

	determine_geometry(message, &width, &height);
	width += 16;
	height += 50;

	if (width < 98)
		width = 98;
	if (symbol != 0)
		pos = 52;
	width += pos;

	if (p_win == NULL) {	/* in the center of the screen */
		x = (guiscreen.width - width) / 2;
		y = (guiscreen.height - height) / 2;
	} else {		/* in the center of the parent window */
		x = p_win->x + (p_win->width - width) / 2;
		y = p_win->y + (p_win->height - height) / 2;
	}
	win = add_window(win_thread, NORMAL_WINDOW, x, y, width, height, "Question", FALSE, TRUE);

	hulp = (char *)malloc(strlen(message) + 1);
	if (hulp == NULL)
		error("Cannot allocate memory in question_dialog().");
        sprintf(hulp, "%s", message);
	line = strtok(hulp, "\n");
	while (line != NULL) {
		add_text(win, NORMAL_TEXT, 5 + pos, 20 + count * 13, line);
		line = strtok(NULL, "\n");
		count++;
	}
	free(hulp);

	add_symbol(win, symbol);

	No = add_button(win, NORMAL_BUTTON, win->width - 49, win->height - 25, 44, 20, "No");
	Yes = add_button(win, NORMAL_BUTTON, 5 + pos, win->height - 25, 44, 20, "Yes");

	create_window(win);
	set_mouse_position(x + (width + pos) / 2, y + Yes->y + 10);
	move_mouse();

	disable_all_windows_except(win);
	while (obj != No && obj != Yes)
		obj = do_windows(win_thread);
	enable_all_windows(win);
	
	result = (obj == Yes);
	delete_window(win, TRUE);
	return result;
}


void message_dialog(GuiWinThread *win_thread, GuiWindow *p_win, char *message, int symbol)
{
	GuiWindow *win;
	GuiObject *obj = NULL, *ok;
	int x, y, width, height, pos = 0, count = 0;
	char *hulp, *line;

	determine_geometry(message, &width, &height);
	width += 16;
	height += 50;
	if (width < 58)
		width = 58;
	if (symbol != 0)
		pos = 52;
	width += pos;
		
	if (p_win == NULL) {	/* in the center of the screen */
		x = (guiscreen.width - width) / 2;
		y = (guiscreen.height - height) / 2;
	} else {		/* in the center of the parent window */
		x = p_win->x + (p_win->width - width) / 2;
		y = p_win->y + (p_win->height - height) / 2;
	}
	win = add_window(win_thread, NORMAL_WINDOW, x, y, width, height, "Message", FALSE, TRUE);

	hulp = (char *)malloc(strlen(message) + 1);
	if (hulp == NULL)
		error("Cannot allocate memory in message_dialog().");
        sprintf(hulp, "%s", message);
	line = strtok(hulp, "\n");
	count = 0;
	while (line != NULL) {
		add_text(win, NORMAL_TEXT, 5 + pos, 20 + count * 13, line);
		line = strtok(NULL, "\n");
		count++;
	}
	free(hulp);
	add_symbol(win, symbol);

	ok = add_button(win, NORMAL_BUTTON, (win->width - pos) / 2 - 22 + pos,
			win->height - 25, 44, 20, "Ok");
	win->enterobj = ok;

	create_window(win);
	set_mouse_position(x + ok->x + ok->width / 2, y + ok->y + ok->height / 2);
	move_mouse();
	
	disable_all_windows_except(win);
	while (obj != ok)
		obj = do_windows(win_thread);
	enable_all_windows(win);

	delete_window(win, TRUE);
}
