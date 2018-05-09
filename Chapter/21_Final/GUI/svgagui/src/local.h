/*************************************************************************
	local.h -- last change: 20-1-1998

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
#pragma once
#include "../svgagui.h"
#include <string.h>

typedef struct GuiCursor
{
	char *cur_bits;
	int cur_width;
	int cur_height;
	
	char *mask_bits;
	int mask_width;
	int mask_height;
	
	int cursor_id;
	int x_hot, y_hot;

	/* variables for X */
#if 0
	Cursor cursor;
#endif
} GuiCursor;


extern GuiCursor cursor[10];
extern Mouse mouse;

/* --------------------- The screen handling functions ------------------ */
void open_svga_screen(int width, int height, int colors);
void close_svga_screen(void);
void update_svga_screen(void);
void init_svga_mouse(void);
void get_svga_mouse_position(void);
void set_svga_mouse_position(int x, int y);
void save_svga_screen_to_xpm(void);
void set_svga_default_palette(int colors);
void set_svga_palette(int color, int red, int green, int blue);
void get_svga_palette(int color, int *red, int *green, int *blue);
int get_svga_message(void);
int get_svga_mouse_button(void);
char get_svga_keyboard_char(void);

void open_X_screen(int width, int height, int colors, char *title);
void close_X_screen(void);
void update_X_screen(void);
void init_X_mouse(void);
void get_X_mouse_position(void);
void set_X_mouse_position(int x, int y);
void set_X_default_palette(int colors);
void set_X_palette(int color, int red, int green, int blue);
void get_X_palette(int color, int *red, int *green, int *blue);
int get_X_message(void);
int get_X_mouse_button(void);
char get_X_keyboard_char(void);
/*void create_X_cursor(int cursor_id);
void switch_X_cursor(int cursor_id);*/

/* --------------------------------------------------------------------- */

extern void error(char *s);

void set_screen_update(int x_min, int y_min, int x_max, int y_max);

void update_button(GuiObject *obj);
void create_button(GuiObject *obj);

void update_image(GuiObject *obj);

void update_browser(GuiObject *obj);

void update_item(GuiObject *obj);
void set_item_focus(GuiObject *obj, int focus, int indent);
void press_pulldown(GuiObject *pulldown);

void update_choice(GuiObject *choice);
void press_choice(GuiObject *choice);

void update_listbox(GuiObject *obj);
void update_listentry(GuiObject *obj);
void press_listbox(GuiObject *obj);

void update_iconlist(GuiObject *obj);
void press_iconlist(GuiObject *obj);

void update_slider(GuiObject *obj);
void create_slider(GuiObject *obj);
void press_slider(GuiObject * slid);

void update_input(GuiObject *obj);
void create_input(GuiObject *obj);
void change_input(GuiObject * input, char ch);
void press_input(GuiObject * input);

void string_to_data(char *label, int x, int y, int width, int height, int color, char *data, int *font_w, int font_h, char **font_data);
void string_to_object(GuiObject * obj, int x_off, int y_off, int element, int type);
void determine_geometry(char *lines, int *width, int *height);
void update_text(GuiObject *obj);
void create_text(GuiObject *obj);

void update_number(GuiObject *obj);
void create_number(GuiObject *obj);

void object_cb(GuiObject * obj, int data);
void object_region(GuiWindow * win);
void add_object(GuiObject * obj);
int get_object_align(GuiObject * obj, int width);
void remove_object(GuiObject * obj, int bg_color);
void check_double_click(GuiObject *obj);

void move_window(GuiWindow * win);

void show_info(GuiObject * obj);
void delete_info(void);

void show_mouse(void);
void delete_mouse(void);

void update_movie(GuiObject * obj);
void show_movie(GuiObject * obj);

void show_listentry_icon(GuiObject * obj);
void update_listentry_icon(GuiObject * obj);
GuiObject *add_text_icon(GuiWindow * win, int type, int x, int y, char *label, char **icon, int icon_xoffs, int icon_yoffs, int heightx);
void set_object_image_data2(GuiObject * obj, char **data, int x_off, int y_off, int pressed, int bg_col, int width, int height, int num_colors, int *character, int *color);
