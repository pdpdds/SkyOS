/*************************************************************************
	window_thread.c -- last change: 6-9-1998

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
#include <string.h>
#include "memory.h"
#include "local.h"
#include "windef.h"
#include "PIT.h"

static int time_count = 0;
static int bring_count = 0, bring_to_top = FALSE;
static GuiObject *info_obj = NULL;


void set_auto_raise(GuiWinThread *win_thread, int auto_raise)
{
	win_thread->auto_raise = auto_raise;
}


void save_background(GuiWinThread *win_thread)
{
	int i;
	char *src, *dst;

	if (win_thread->background != NULL)
		free(win_thread->background);

	win_thread->background = (char *)malloc(guiscreen.width * guiscreen.height);
	if (win_thread->background == NULL)
		error("Cannot allocate memory in create_window_thread().");
	src = guiscreen.data;
	dst = win_thread->background;
	delete_mouse();
	for (i = 0;i < guiscreen.height;i++) {
		memcpy(dst, src, guiscreen.width);
		dst += guiscreen.width;
		src += guiscreen.width;
	}
	show_mouse();

}


GuiWinThread *create_window_thread(void)
{
	GuiWinThread *win_thread;

	win_thread = (GuiWinThread *) malloc(sizeof(GuiWinThread));
	if (win_thread == NULL)
		error("Cannot allocate window thread.");
	win_thread->first = NULL;
	win_thread->number = 0;
	win_thread->focuswin = NULL;
	win_thread->oldfocuswin = NULL;
	win_thread->background = NULL;
	win_thread->show_window_thread = FALSE;
	win_thread->auto_raise = TRUE;
	win_thread->show_tips = TRUE;

	/* add a tip window */
	win_thread->tip_win = add_window(win_thread, TIP_WINDOW, 0, 0, 2, 16, "", FALSE, TRUE);

	/* save the background image */
	save_background(win_thread);
	
	return win_thread;
}


void delete_window_thread(GuiWinThread * win_thread)
{
	GuiWindow *win = win_thread->first;

	while (win != NULL) {
		delete_window(win, TRUE);
		win = win_thread->first;
	}
	free(win_thread->background);
	free(win_thread);
}


void show_window_thread(GuiWinThread * win_thread)
{
	GuiWindow *tmp_win;
	int z = win_thread->number - 1;
	
	win_thread->show_window_thread = TRUE;
	while (z >= 0) {
		tmp_win = win_thread->first;
		while (tmp_win->z_order != z) {
			tmp_win = tmp_win->next;
			//if (tmp_win == NULL)
				//fprintf(stderr, "---> Warning: window (z = %d) does not exist.\n", z);
		}
		if (tmp_win != NULL && !tmp_win->hide) {
			set_maximum_update_region(tmp_win);
			copy_window_to_screen_image(tmp_win, FALSE, TRUE, FALSE);
		}
		z--;
	}
	update_screen();

	/* reset the bring window to top counters */
	bring_to_top = FALSE;
	bring_count = 0;
}


void do_window_functions(GuiWinThread *win_thread)
{
	GuiWindow *win = win_thread->first;
	
	while (win != NULL) {
		if (!win->hide)
			win->function();
		win = win->next;
	}
}


static int in_title(GuiWindow * win)
{
	if (check_window(win, "in_title"))
		return FALSE;

	if (win->type == NORMAL_WINDOW) {
		if (mouse.x >= win->x && mouse.x <= win->x + win->width &&
		    mouse.y >= win->y && mouse.y < win->y + 18)
			return TRUE;
	}
	return FALSE;
}

static GuiObject *in_window(GuiWindow * win)
{
	int button_touched = FALSE;
	GuiObject *obj;
	
	if (check_window(win, "in_window"))
		return NULL;

	object_region(win);
	if (win->focusobj != NULL && info_obj == NULL) {
		if (strlen(win->focusobj->info) > 0) {	/* mouse on object */
			time_count++;
			if (time_count > 50) {
				show_info(win->focusobj);
				info_obj = win->focusobj;
			}
		}
	} else
		time_count = 0;

	if (info_obj != NULL && info_obj != win->focusobj) {
		delete_info();
		info_obj = NULL;
	}
	if (GuiMouseGetButton() == GuiMouseLeftButton) {
		if (win->focusobj != NULL && win->focusobj->active)
			button_touched = TRUE;
	}
	if (button_touched) {
		if (info_obj != NULL) {
			delete_info();
			info_obj = NULL;
		}
		obj = win->focusobj;
	} else
		obj = NULL;

	return obj;
}


static int wait_for_mouse(GuiObject * obj)
{
	GuiWindow *win;
	int status, was_pressed;

	check_object(obj, "obj", "wait_for_mouse");
	win = obj->win;
	check_window(win, "wait_for_mouse");
	was_pressed = obj->pressed;

	show_object(obj);
	status = GuiMouseGetButton();
	while (status != 0) {
		ksleep(sleep_time);
		do_window_functions(win->win_thread);
		if (GuiGetMessage()) {
			move_mouse();
			status = GuiMouseGetButton();
			if (obj->objclass == BUTTON) {
				object_region(win);
				if (!was_pressed) {
					if (win->focusobj != obj && obj->pressed)
						show_object(obj);
					if (win->focusobj == obj && !obj->pressed)
						show_object(obj);
				} else {
					if (win->focusobj != obj && !obj->pressed)
						show_object(obj);
					if (win->focusobj == obj && obj->pressed)
						show_object(obj);
				}
			}
		}
		if (!obj->wait_for_mouse && obj->pressed)
			obj->object_callback(obj, obj->u_data);
	}
	if (win->focusobj == obj)
		return TRUE;
	else
		return FALSE;
}


static void update_wintitle(GuiWindow * win, int focus)
{
	if (check_window(win, "update_wintitle"))
		return;

	if (focus) {
		win_fillbox(win, 3, 3, win->width - 6, 15, ACTIVE_TITLE_BACK);
		set_object_color(win->title, ACTIVE_TITLE_BACK,
				 ACTIVE_TITLE_BACK , ACTIVE_TITLE_FORE);
	} else {
		win_fillbox(win, 3, 3, win->width - 6, 15, TITLE_BACK);
		set_object_color(win->title, TITLE_BACK, TITLE_BACK, TITLE_FORE);
	}
	if (win->kill != NULL)
		update_object(win->kill);
	update_object(win->title);
	show_window(win);
}


void bring_window_to_top(GuiWindow * win)
{
	GuiWindow *tmp_win, *first_win;
	int z_old, depth = 0;

	if (check_window(win, "bring_window_to_top"))
		return;
	if (!win->active)
		return;
	first_win = (win->win_thread)->first;
	z_old = win->z_order;	
		
	if (win->always_on_top) {
		if (win->z_order != 0) {
			/* Reorganise the z-order for the always on top windows */
			for (tmp_win = first_win; tmp_win != NULL; tmp_win = tmp_win->next)
				if (tmp_win->always_on_top && tmp_win->z_order < z_old)
					tmp_win->z_order++;
			win->z_order = 0;
		}
	} else {
		depth = 0;
		for (tmp_win = first_win; tmp_win != NULL; tmp_win = tmp_win->next)
			if (tmp_win->always_on_top)
				depth++;
		for (tmp_win = first_win; tmp_win != NULL; tmp_win = tmp_win->next)
			if (tmp_win->z_order < z_old && !tmp_win->always_on_top)
				tmp_win->z_order++;
		win->z_order = depth;
	}

	/* show the current window */
	set_maximum_update_region(win);
	copy_window_to_screen_image(win, FALSE, TRUE, TRUE);
	update_screen();
}


static void window_region(GuiWinThread * win_thread)
{
	int found = FALSE, z = 0;
	GuiWindow *win = NULL, *old_win = win_thread->focuswin;
	GuiWindow *old_focus_win = win_thread->oldfocuswin;

	while (z < win_thread->number && !found) {
		win = win_thread->first;
		while (win->z_order != z)
			win = win->next;
		z++;
		if (mouse.x >= win->x && mouse.x < win->x + win->width &&
		    mouse.y >= win->y && mouse.y < win->y + win->height &&
		    !win->hide)
			found = TRUE;
	}
	if (win != old_focus_win && old_focus_win != NULL &&
	    info_obj != NULL) {
		delete_info();
		info_obj = NULL;
	}
	if (found) {
		if (win != old_focus_win) {
			if (old_focus_win != NULL && (old_focus_win)->type == NORMAL_WINDOW)
				update_wintitle(old_focus_win, FALSE);
			if (old_focus_win != NULL && (old_focus_win)->inputfocus != NULL) {
				GuiObject *obj = old_focus_win->inputfocus;
				
				old_focus_win->inputfocus = NULL;
				create_input(obj);
				show_input(obj);
			}
			if (win->type == NORMAL_WINDOW)
				update_wintitle(win, TRUE);
			win_thread->oldfocuswin = win;
		}
		win_thread->focuswin = win;
		if (win != old_win && win != NULL) {
			bring_count = 0;
			bring_to_top = TRUE;
		}
	} else
		win_thread->focuswin = NULL;
}


GuiObject *do_windows(GuiWinThread * win_thread)
{
	GuiWindow *win;
	GuiObject *obj = NULL, *input;
	char ch;
	int message = 0;

	window_region(win_thread);
	do {
		ksleep(sleep_time);
		do_window_functions(win_thread);
		if (win_thread->focuswin != NULL) {
			win = win_thread->focuswin;
			obj = in_window(win);
			if (in_title(win) && (obj != win->kill || win->kill == NULL) &&
			    GuiMouseGetButton() == GuiMouseLeftButton) {
				if (bring_to_top) {
					bring_window_to_top(win_thread->focuswin);
					bring_to_top = FALSE;
				}
				move_window(win);
			}
			
			if (obj == win->title && obj != NULL)
				obj = NULL;
			if (obj != NULL) {
				if (obj->objclass == BUTTON)
					if (!wait_for_mouse(obj))
						obj = NULL;
			}
		}
		message = GuiGetMessage();
		switch (message) {
			case GuiMouseEvent:
				move_mouse();
				window_region(win_thread);
				break;
			case GuiKeyboardEvent:
				ch = GuiKeyboardGetChar();
				switch (ch) {

				case 19:	/* CTRL-s pressed */
					if (guiscreen.type == SVGALIB)
						save_svga_screen_to_xpm();
					break;
				case 13:	/* enter pressed Xlib */
				case 10:	/* enter pressed Svgalib */
					if (win_thread->oldfocuswin == NULL)
						break;
					input = (win_thread->oldfocuswin)->inputfocus;
					if (input != NULL)
						input->object_callback(input, input->u_data);
					else {
						obj = (win_thread->oldfocuswin)->enterobj;
						if (obj == NULL)
							break;
						if (obj->wait_for_mouse && obj->objclass == BUTTON)
							show_button(obj);
						if (info_obj != NULL) {
							delete_info();
							info_obj = NULL;
						}
					}
				}
				if (win_thread->oldfocuswin != NULL &&
				    (win_thread->oldfocuswin)->inputfocus != NULL)
					change_input((win_thread->oldfocuswin)->inputfocus, ch);
				break;
			default:
				break;
		}
		if (bring_to_top && win_thread->auto_raise && win_thread->focuswin != NULL) {
			bring_count++;
			if (bring_count > 20) {
				bring_window_to_top(win_thread->focuswin);
				bring_to_top = FALSE;
			}
		}
	}
	while (obj == NULL);
	if (bring_to_top && win_thread->auto_raise && win_thread->focuswin != NULL) {
		bring_window_to_top(win_thread->focuswin);
		bring_to_top = FALSE;
	}

	if (obj->objclass == BUTTON) {
		if (!obj->wait_for_mouse)
			show_button(obj);
		obj->object_callback(obj, obj->u_data);
	}
	if (obj->objclass == SLIDER)
		press_slider(obj);
	if (obj->objclass == INPUT)
		press_input(obj);
	if (obj->objclass == PULLDOWN)
		press_pulldown(obj);
	if (obj->objclass == CHOICE)
		press_choice(obj);
	if (obj->objclass == LISTENTRY)
		press_listbox(obj);
	if (obj->objclass == LISTENTRY_ICON)
		press_iconlist(obj);
		
	return obj;
}
