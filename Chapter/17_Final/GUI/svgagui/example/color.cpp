/************************************************************************
	color.c -- last change: 2-1-2007

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
#include "svgagui.h"
#include "svgaguientry.h"


static void select_color_cb(GuiObject * obj, int data)
{
	int r, g, b;
	
	kleur = data;
	get_gui_palette(kleur, &r, &g, &b);
	set_slider_position(color[0], (int)(r));
	set_slider_position(color[1], (int)(g));
	set_slider_position(color[2], (int)(b));
	win_fillbox(color_win, 100, 70, 40, 30, kleur);
	win_3dbox(color_win, DOWN_FRAME, 100, 70, 40, 30);
	show_window(color_win);
}


static void set_color_cb(GuiObject * obj, int data)
{
	int r, g, b;
	
	r = (int)(color[0]->position);
	g = (int)(color[1]->position);
	b = (int)(color[2]->position);
	set_gui_palette(kleur, r, g, b);
}


void default_cb(GuiObject * obj, int data)
{
	if (question_dialog((obj->win)->win_thread, NULL, "Restore default color setting?", DIA_QUESTION)) {
		set_default_palette();
		select_color_cb(NULL, kleur);
	}
}


void load_cb(GuiObject * obj, int data)
{
	/*FILE *invoer;
	int i, r, g, b;
	char hulp[50];
	
	sprintf(hulp, "%s/.svgaguicolors", getenv("HOME"));	
	invoer = fopen(hulp, "r");
	if (invoer != NULL) {
		for (i = 0; i < 25; i++) {
			fscanf(invoer, "%d %d %d", &r, &g, &b);
			set_gui_palette(i, r, g, b);
		}
		select_color_cb(NULL, kleur);
		fclose(invoer);
		message_dialog((obj->win)->win_thread, NULL, "Color settings loaded", DIA_INFO);
	}*/
}


void save_cb(GuiObject * obj, int data)
{
	/*FILE *uitvoer;
	int i, r, g, b;
	char hulp[50];

	sprintf(hulp, "%s/.svgaguicolors", getenv("HOME"));	
	uitvoer = fopen(hulp, "w");
	if (uitvoer != NULL) {
		for (i = 0;i < 25;i++) {
			get_gui_palette(i, &r, &g, &b);
			fprintf(uitvoer, "%2d %2d %2d\n", r, g, b);
		}
		fclose(uitvoer);
		message_dialog((obj->win)->win_thread, NULL, "Color settings saved", DIA_INFO);
	}*/
}


static void create_color_choice(GuiWindow *win)
{
	GuiObject *obj, *pd;
	
	pd = add_choice(win, 6, 24, 130);
	  obj = add_item(pd, "Background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, BACKGROUND);
	  obj = add_item(pd, "Window background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, WIN_BACK);
	  obj = add_item(pd, "Title background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, TITLE_BACK);
	  obj = add_item(pd, "Title foreground", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, TITLE_FORE);
	  obj = add_item(pd, "Active title background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, ACTIVE_TITLE_BACK);
	  obj = add_item(pd, "Active title foreground", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, ACTIVE_TITLE_FORE);
	  obj = add_item(pd, "Normal text", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, TEXT_NORMAL);
	  obj = add_item(pd, "Browser background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, BROWSER_BACK);
	  obj = add_item(pd, "Browser foreground", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, BROWSER_FORE);
	  obj = add_item(pd, "Input background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, INPUT_BACK);
	  obj = add_item(pd, "Input foreground", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, INPUT_FORE);
	  obj = add_item(pd, "Active input background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, ACTIVE_INPUT_BACK);
	  obj = add_item(pd, "Button background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, BUTTON_BACK);
	  obj = add_item(pd, "Button foreground", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, BUTTON_FORE);
	  obj = add_item(pd, "Slider background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, SLIDER_BACK);
	  obj = add_item(pd, "Slider foreground", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, SLIDER_FORE);
	  obj = add_item(pd, "Choice background", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, CHOICE_BACK);
	  obj = add_item(pd, "Number foreground", NORMAL_ITEM);
	  set_object_callback(obj, select_color_cb);
	  set_object_user_data(obj, NUMBER_FORE);
	create_choice(pd);
}


void create_color_win(GuiWinThread * win_thread)
{
	int r, g, b;
	GuiWindow *win;
	GuiObject *obj;

	win = add_window(win_thread, NORMAL_WINDOW, 340, 50, 160, 135, "Colors", FALSE, FALSE);
	  color_win = win;
	  create_color_choice(win);
	  add_text(win, NORMAL_TEXT, 98, 55, "Example:");

	  obj = add_slider(win, NICE_VALUE_SLIDER, 10, 60, 64, FALSE);

	    color[0] = obj;
	    set_object_callback(obj, set_color_cb);
	    set_slider_maxvalue(obj, 63);
	    set_object_color(obj, WIN_BACK, SLIDER_BACK, RED);		

	  obj = add_slider(win, NICE_VALUE_SLIDER, 40, 60, 64, FALSE);
	    color[1] = obj;
	    set_object_callback(obj, set_color_cb);
	    set_slider_maxvalue(obj, 63);
	    set_object_color(obj, WIN_BACK, SLIDER_BACK, GREEN);
	  obj = add_slider(win, NICE_VALUE_SLIDER, 70, 60, 64, FALSE);
	    color[2] = obj;
	    set_object_callback(obj, set_color_cb);
	    set_slider_maxvalue(obj, 63);
	    set_object_color(obj, WIN_BACK, SLIDER_BACK, BLUE);

	create_window(win);	

	/* set the current color (sliders etc.) */
	get_gui_palette(kleur, &r, &g, &b);
	set_slider_position(color[0], (int)(r));
	set_slider_position(color[1], (int)(g));
	set_slider_position(color[2], (int)(b));
	win_fillbox(color_win, 100, 70, 40, 30, kleur);
	win_3dbox(color_win, DOWN_FRAME, 100, 70, 40, 30);
}
