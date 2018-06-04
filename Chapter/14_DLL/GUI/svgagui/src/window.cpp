/*************************************************************************
	window.c -- last change: 7-12-1998

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
#include "string.h"
#include "local.h"
#include "windef.h"
#include "PIT.h"
#include "images/kill.xpm"

/* allocate memory to store the disable levels */
int disable_count = 0;
int *disable_win[10];	/* 10 disable levels */


void set_maximum_update_region(GuiWindow *win)
{
	check_window(win, "set_maximum_update_region");

	win->x_min = 0;
	win->x_max = win->width - 1;
	win->y_min = 0;
	win->y_max = win->height - 1;
}


static void update_window(GuiWindow * win, int x_old, int y_old,
			  int x_new, int y_new)
{
	check_window(win, "update_window");

	if (x_old == x_new && y_old == y_new)	/* window has not moved */
		return;

	/* remove the window from the screen */
	set_maximum_update_region(win);
	copy_window_to_screen_image(win, TRUE, TRUE, TRUE);

	win->x = x_new;
	win->y = y_new;
	/* place the window on the new position */
	set_maximum_update_region(win);
	copy_window_to_screen_image(win, FALSE, TRUE, TRUE);
	update_screen();
}


void move_window(GuiWindow * win)
{
	GuiWinThread *win_thread;
	int x_off, y_off;
	int x, y, x_old, y_old;

	check_window(win, "move_window");
	win_thread = win->win_thread;
	x_off = win->x - mouse.x;
	y_off = win->y - mouse.y;
	x_old = win->x;
	y_old = win->y;

	do {
		ksleep(sleep_time / 5);
		do_window_functions(win_thread);
		if (GuiGetMessage() == GuiMouseEvent) {
			move_mouse();
			x = mouse.x + x_off;
			y = mouse.y + y_off;
			update_window(win, x_old, y_old, x, y);
			x_old = x;
			y_old = y;
		}
	}
	while (GuiMouseGetButton() == GuiMouseLeftButton);
}

void create_window(GuiWindow * win)
{
	check_window(win, "create_window");

	win_fillbox(win, 0, 0, win->width, win->height, win->bg_col);
	win_3dbox(win, UP_FRAME, 0, 0, win->width, win->height);
	if (win->type == NORMAL_WINDOW)
		win_fillbox(win, 3, 3, win->width - 6, 15, TITLE_BACK);

	update_objects(win);
}


void activate_window(GuiWindow *win, int active)
{
	check_window(win, "activate_window");
	
	win->active = active;
}


void set_window_function(GuiWindow *win, void (*function) (void) )
{
	check_window(win, "set_window_function");
	
	win->function = function;
}


static void winfunction(void)
{
}


void reset_window_function(GuiWindow *win)
{
	check_window(win, "reset_window_function");
	
	win->function = winfunction;
}


void disable_all_windows_except(GuiWindow *this_win)
{
	GuiWinThread *win_thread = this_win->win_thread;
	GuiWindow *win, *tip_win;
	int count;
	
	check_window(this_win, "disable_all_windows_except");
	
	/* store the active window information */
	disable_win[disable_count] = (int *)malloc(win_thread->number * sizeof(int));
	if (disable_win[disable_count] == NULL)
		error("Unable to allocate memory in disable_all_windows_except().");
	
	win = win_thread->first;
	tip_win = (this_win->win_thread)->tip_win;
	count = 0;
	while (win != NULL) {
		disable_win[disable_count][count++] = win->active;
		activate_window(win, ((win == this_win) || (win == tip_win)) ? TRUE : FALSE);
		win = win->next;
	}
	disable_count++;
	if (disable_count > 9)
		disable_count = 9;
}


void enable_all_windows(GuiWindow *this_win)
{
	GuiWindow *win;
	int count;
	
	check_window(this_win, "enable_all_windows");
	disable_count--;
	if (disable_count < 0)
		disable_count = 0;
	win = (this_win->win_thread)->first;
	count = 0;
	while (win != NULL) {
		activate_window(win, disable_win[disable_count][count++]);
		win = win->next;
	}
	free(disable_win[disable_count]);
}


void show_window(GuiWindow * win)
{
	check_window(win, "show_window");

	if ((win->x_min == -1) || (win->x_max == -1))
		return;
	if ((win->y_min == -1) || (win->y_max == -1))
		return;
	if (win->hide)
		return;

	bool result = win->win_thread->show_window_thread;
	if (!result)
//		fprintf(stderr, "----> Warning: show_window() called before show_window_thread()!\n");

	copy_window_to_screen_image(win, FALSE, TRUE, TRUE);
	update_screen();
}


void delete_window(GuiWindow * win, int update)
{
	GuiWinThread *win_thread;
	GuiWindow *tmp_win;
	GuiObject *obj;
	int z_old;

	check_window(win, "delete_window");
	win_thread = win->win_thread;
	obj = win->first;
	z_old = win->z_order;

	/* remove the window from the screen */
	if (!win->hide && update) {
		set_maximum_update_region(win);
		copy_window_to_screen_image(win, TRUE, TRUE, TRUE);
		update_screen();
	}

	if (win_thread->focuswin == win)	
		win_thread->focuswin = NULL;
	if (win_thread->oldfocuswin == win)
		win_thread->oldfocuswin = NULL;

	/* update the window thread */
	if (win_thread->first == win) {		/* first window */
		if (win->next != NULL) {
			win_thread->first = win->next;
			(win->next)->prev = NULL;
		} else
			win_thread->first = NULL;
	} else if (win->next == NULL)	/* last element */
		(win->prev)->next = NULL;
	else {
		(win->next)->prev = win->prev;
		(win->prev)->next = win->next;
	}
	win_thread->number--;	/* decrease the number of windows in thread */

	/* update z-order */
	for (tmp_win = win_thread->first;tmp_win != NULL;tmp_win = tmp_win->next)
		if (tmp_win->z_order > z_old)
			tmp_win->z_order--;
	
	/* delete allocated memory */
	while (obj != NULL) {
		delete_object(obj);
		obj = win->first;
	}
	free(win->data);
	free(win);
}


void hide_window(GuiWindow * win, int update)
{
	check_window(win, "hide_window");

	if (win->hide)
		return;
	/* remove the window image */
	set_maximum_update_region(win);
	copy_window_to_screen_image(win, TRUE, TRUE, TRUE);
	if (update)
		update_screen();

	win->hide = TRUE;
}


void unhide_window(GuiWindow * win, int update)
{
	check_window(win, "unhide_window");

	if (!win->hide)
		return;
	/* show the window again */
	set_maximum_update_region(win);
	copy_window_to_screen_image(win, FALSE, TRUE, TRUE);
	if (update)
		update_screen();

	win->hide = FALSE;
}


GuiWindow *add_window(GuiWinThread * win_thread, int type, int x, int y,
		   int width, int height, char *title, int kill_button,
		   int always_on_top)
{
	GuiWindow *win, *win_list = win_thread->first, *tmp_win;
	GuiObject *obj;
	int color, depth;

	win = (GuiWindow *) malloc(sizeof(GuiWindow));
	if (win == NULL)
		error("Cannot allocate memory for window.");

	win->win_thread = win_thread;
	win->function = winfunction;
	win->x = x;
	win->y = y;
	win->width = width;
	win->height = height;
	win->type = type;
	win->hide = type == TIP_WINDOW ? TRUE : FALSE;
	win->number = 0;
	win->first = NULL;	/* the first object of this window */
	win->kill = NULL;
	win->title = NULL;
	win->inputfocus = NULL;
	win->enterobj = NULL;
	win->userobj1 = NULL;
	win->userobj2 = NULL;
	win->bg_col = WIN_BACK;
	win->active = TRUE;
	set_maximum_update_region(win);

	/* allocate memory for the window image */
	win->data = (char *) malloc(width * height);
	if (win->data == NULL)
		error("Cannot allocate window buffers in add_window().");
	if (win->type == NORMAL_WINDOW) {
		if (strlen(title) > 0) {
			obj = add_text(win, NORMAL_TEXT, 4, 4, title);
			win->title = obj;
			set_object_color(obj, TITLE_BACK, TITLE_BACK, TITLE_FORE);
		}
		if (kill_button) {
			win->kill = add_button(win, PIXMAP_BUTTON, win->width - 16,
					       5, 12, 12, "");
			color = (win->kill)->bg_col1;
			set_object_image_data(win->kill, kill_xpm, 2, 2, 0, color);
			set_object_image_data(win->kill, kill_xpm, 3, 3, 1, color);
		}
	}
	/* add window to the window thread */
	if (win_list == NULL) {	/* first window to add */
		win_thread->first = win;
		win->next = NULL;
		win->prev = NULL;
	} else {
		win_list = win_thread->first;
		while (win_list->next != NULL)	/* go to last window in list */
			win_list = win_list->next;
		win_list->next = win;
		win->next = NULL;
		win->prev = win_list;
	}
	win->always_on_top = always_on_top;
	win_thread->number++;	/* increase number of windows */

	/* change the z-order of the windows */
	if (win->always_on_top) {
		/* Reorganise the z-order for the always on top windows */
		for (tmp_win = win_thread->first; tmp_win != NULL; tmp_win = tmp_win->next)
			if (tmp_win != win)
				tmp_win->z_order++;
		win->z_order = 0;
	} else {
		depth = 0;
		for (tmp_win = win_thread->first; tmp_win != NULL; tmp_win = tmp_win->next)
			if (tmp_win->always_on_top)
				depth++;
		for (tmp_win = win_thread->first; tmp_win != NULL; tmp_win = tmp_win->next)
			if (!tmp_win->always_on_top && tmp_win != win)
				tmp_win->z_order++;
		win->z_order = depth;
	}

	return win;
}
