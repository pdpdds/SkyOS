/*************************************************************************
	progress.c -- last change: 7-12-1998

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
#include "local.h"
#include "windef.h"
#include "PIT.h"
#include "sprintf.h"

static GuiObject *perc_obj;
static int old_cursor;
static int progress_ok = FALSE;
static float old_percent;


GuiWindow *init_progress(GuiWinThread *win_thread, char *string)
{
	GuiWindow *win;
	int width = 212, height = 80;
	int x, y = (guiscreen.height - height) / 2;

	if (progress_ok) {
		//fprintf(stderr, "---> Warning: progress() already exists.\n");
		return NULL;
	}
	progress_ok = TRUE;
	old_cursor = mouse.cursor_id;
	old_percent = 0.0;
	switch_cursor(WATCH_CURSOR);
	if (string_length(string) + 10 > width)
		width = string_length(string) + 10;
	x = (guiscreen.width - width) / 2;

	win = add_window(win_thread, NORMAL_WINDOW, x, y, width, height,
			 "Progress indication", FALSE, TRUE);

	add_text(win, NORMAL_TEXT, 4, 25, string);
	add_text(win, NORMAL_TEXT, 4, 45, "Progress:");
	perc_obj = add_text(win, NORMAL_TEXT, 60, 45, "100%");
	create_window(win);
	win_fillbox(win, 6, 64, 200, 10, WHITE);
	win_3dbox(win, DOWN_FRAME, 4, 62, 204, 14);
	show_window(win);
	disable_all_windows_except(win);
	
	return win;
}


void show_progress(GuiWindow *win, float percent)
{
	char hulp[20];
	
	if (!progress_ok) {
//		fprintf(stderr, "---> Warning: show_progress() called, but progress does not exist.\n");
		return;
	}
	if (percent - old_percent < 0.01)
		return;
	old_percent = percent;
	win_fillbox(win, 6, 64, 2 * (int) (percent * 100), 10, DARKBLUE);
	sprintf(hulp, "%0.0f%%", percent * 100);
	set_text(perc_obj, hulp);
	show_text(perc_obj);
	if (GuiGetMessage() == GuiMouseEvent)
		move_mouse();
	do_window_functions(win->win_thread);
}


void delete_progress(GuiWindow *win)
{
	if (!progress_ok) {
//		fprintf(stderr, "---> Warning: delete_progress() called, but progress does not exist.\n");
		return;
	}
	progress_ok = FALSE;
	enable_all_windows(win);
	delete_window(win, TRUE);
	switch_cursor(old_cursor);
}
