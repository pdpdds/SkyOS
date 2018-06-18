/************************************************************************
	svgagui.h -- last change: 2-1-2007

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
#include "kheap.h"

#ifndef FALSE
#define FALSE       0
#endif
#ifndef TRUE
#define TRUE        1
#endif

/**
Implementation is done in LikeOS kernel
***/

// define the colorscheme.
extern unsigned long	BACKGROUND, WIN_BACK,
		TITLE_BACK, TITLE_FORE,
		ACTIVE_TITLE_FORE, ACTIVE_TITLE_BACK,
		BUTTON_BACK, BUTTON_FORE,
		SLIDER_BACK, SLIDER_FORE,
		TEXT_EMBOSS, TEXT_NORMAL, TEXT_HIGHLIGHT,
		INPUT_BACK, INPUT_FORE, ACTIVE_INPUT_BACK,
		BROWSER_BACK, BROWSER_FORE,
		INFO_BACK, INFO_FORE,
		LISTBOX_BACK, LISTBOX_FORE,
		CHOICE_BACK, CHOICE_FORE,
		NUMBER_FORE;

// Define the extra colors.
extern unsigned long	BLACK, DARKGREY, GREY, LIGHTGREY, WHITE,
		DARKRED, RED, DARKBLUE, BLUE, LIGHTBLUE,
		DARKGREEN, GREEN, DARKYELLOW, YELLOW;

// Define the objects.
#define BUTTON		0
#define SLIDER		1
#define WINTEXT		2
#define NUMBER		3
#define INPUT		4
#define IMAGE		5
#define BROWSER		6
#define PULLDOWN	7
#define ITEM		8
#define CHOICE		9
#define LISTBOX		10
#define LISTENTRY	11
#define MOVIE		12
#define ICONLIST	13
#define LISTENTRY_ICON 14

// Define the different window types.
#define NORMAL_WINDOW		0
#define NO_TITLE_WINDOW		1
#define TIP_WINDOW			2

// Define the different buttons.
#define NORMAL_BUTTON		0
#define LIGHT_BUTTON		1
#define PIXMAP_BUTTON		2
#define CHECK_BUTTON		3
#define RADIO_BUTTON		4

// Define the 4 different slider types.
#define VERT_SLIDER			0
#define HOR_SLIDER			1
#define NICE_HOR_SLIDER		2
#define NICE_VERT_SLIDER 	3
#define NICE_VALUE_SLIDER	4

// Define the text type.
#define NORMAL_TEXT			0
#define EMBOSSED_TEXT		1

// Define the input type.
#define NORMAL_INPUT		0
#define FLOAT_INPUT			1
#define INT_INPUT			2

// Define the browser type.
#define NORMAL_BROWSER		0

// Define the pulldown type.
#define NORMAL_PULLDOWN		0

// Define the item type.
#define NORMAL_ITEM			0
#define CHECK_ITEM			1
#define POPUP_ITEM			2

// Define the number type.
#define NORMAL_NUMBER		0

// Align text or images.
#define ALIGN_LEFT			0
#define ALIGN_RIGHT			1
#define ALIGN_CENTER		2

// Frame types.
#define NO_FRAME			0
#define UP_FRAME			1
#define DOWN_FRAME			2
#define FLAT_FRAME			3

// Dialog symbols.
#define DIA_EXCLAMATION		1
#define DIA_INFO			2
#define DIA_QUESTION		3
#define DIA_STOP			4

// Define the cursors.
#define DEFAULT_CURSOR		0
#define WATCH_CURSOR		1

// Define the mouse buttons.
#define GuiMouseLeftButton	1
#define GuiMouseMiddleButton	2
#define GuiMouseRightButton	3

// Define the events.
#define GuiMouseEvent		1
#define GuiKeyboardEvent	2

// Define the libraries for the interface.
#define SVGALIB			1
#define XLIB			2

struct GuiWindow_ ;
struct GuiWinThread_ ;

typedef struct GuiObject_ {
	struct GuiWindow_ *win;	// window where object belongs to.
	int x, y;				// position of the object.
	int width, height;		// width and height of the object.
	int active;				// interaction?.
	int pressed;			// is the object pressed?.
	int hide;				// is the object hidden?.
	int wait_for_mouse;		// wait for user to release button?.
	int u_data;				// data defined by the user.

	void (*object_callback) (struct GuiObject_ * obj, int parameter);	// callback function.
	void (*object_callback2) (struct GuiObject_ * obj, int parameter);	// callback function.
	void (*object_init) (struct GuiObject_ *obj);
	int objclass;		// object class: button, slider, etc..
	int type;			// what type.

	int fg_col, bg_col1, bg_col2;	// colors of the object.
	int frame_type;		// rectangle around the object.
	char info[128];		// tip window information.
	char label[256];	// label of the object.
	int align;			// alignment of the image or text.

	int x_min, x_max;	// x boundaries for action.
	int y_min, y_max;	// y boundaries for action.

	// two pixmaps for the object.
	char *data[2];		// data blocks for the pixmaps.

	// for the sliders.
	int slider_length;	// total slider length.
	int position;		// current position of the slider.
	int length;			// length of the slider bar.
	int buttons;		// slider buttons, move left, right, up, down.
	int middle;			// place red line in middle.
	float max_value;	// maximum value of the slider.

	// for the browser.
	int b_width, b_height;
	int sliders;
	int b_x, b_y;
	char buffer[65535];
	
	// for the pulldown menu's.
	int sep_count;
	int sep[10];

	// for the listbox.
	int nr_items;		// number of visible items in the listbox.
	int tot_nr_items;	// total number of items in the box.
	int item_pos;		// start item of the box.

	// for the object list.
	struct GuiObject_ *next;		// pointer to next object.
	struct GuiObject_ *prev;		// pointer to previous object.
	struct GuiObject_ *obj_link;	// pointer to a linked object.
	struct GuiWindow_ *win_link;	// pointer to a linked window.
} GuiObject;


typedef struct GuiWindow_ {
	struct GuiWinThread_ *win_thread;	// the thread this window belows.
	void (*function) (void);
	int x, y;
	int z_order;		// 'height' of the window.
	int always_on_top;	// this window is always on top.
	int active;			// is the window active?.
	int width, height;
	int type;
	int hide;
	int bg_col;
	int number;		// number of objects in the window.

	GuiObject *first;		// first element of the objects.
	GuiObject *focusobj;	// object where the mouse is positioned.
	GuiObject *inputfocus;	// input that will recieve characters.
	GuiObject *enterobj;	// object that will be pressed in enter-key is pressed.
	GuiObject *kill;		// kill button in title bar of window.
	GuiObject *title;		// the title of the window.
	GuiObject *userobj1;	// an object that can be defined by the user.
	GuiObject *userobj2;	// an object that can be defined by the user.

	char *data;		// pixmap of the window.

	// which region to update?.
	int x_min, x_max;
	int y_min, y_max;

	// for the window list.
	struct GuiWindow_ *next;
	struct GuiWindow_ *prev;
} GuiWindow;


typedef struct GuiWinThread_ {
	GuiWindow *first;		// first window of the thread.
	int number;				// number of windows in the thread.
	GuiWindow *focuswin;	// window with the current focus.
	GuiWindow *oldfocuswin;	// window with the last focus.
	GuiWindow *tip_win;		// window for a tip.

	int show_window_thread;	// FALSE if the function is not called yet.
	char *background;		// to store the background image.
	int auto_raise;			// if TRUE: auto raise on mouse focus.
	int show_tips;			// if TRUE: show tip windows.
} GuiWinThread;


typedef struct Mouse {
	int x, y;			// the mouse position.
	int visible;		// is the mouse visible?.
	int cursor_id;		// The id of the current cursor.
	int num_cursors;	// The number of available cursors.
	char *savescreen;	// save the background of the mouse cursor.
	void (*show_function) (void);
	void (*delete_function) (void);
} Mouse;


#if 0
#include <X11/Xlib.h>
#endif

typedef struct GuiScreen {
	// Memory for the screen image.
	char *data;
	// which region to update.
	int x_min, x_max;
	int y_min, y_max;

	int width, height;
	int colors;

	int type;
#if 0
	// Variables for X.
	Display *display;
	int screen_num;
	Window win;
	Colormap cmap;
	GC gc;
	XEvent event;
	XImage *image;
	long event_mask;
	unsigned long *image_data;
#endif
} GuiScreen;


extern Mouse mouse;
extern GuiScreen guiscreen;
extern int svgagui_initialized, sleep_time;

extern int _clip, _clipx1, _clipx2, _clipy1, _clipy2;

void error(char *s);
void open_screen(int type, int width, int height, int colors, char *title);
void close_screen();
void set_default_palette(void);
void set_gui_palette(int color, int red, int green, int blue);
void get_gui_palette(int color, int *red, int *green, int *blue);

void switch_cursor(int cursor_id);
void create_cursor(int cursor_id);
int add_new_cursor(unsigned char *cur_bits, int cur_width, int cur_height,
		   unsigned char *mask_bits, int mask_width, int mask_height,
		   int x_hot, int y_hot);
void reset_mouse(void);
void init_mouse(void);
void move_mouse(void);
void set_mouse_position(int x, int y);

GuiObject *add_button(GuiWindow * win, int type, int x, int y, int width, int height, char *label);
void show_button(GuiObject * but);
void set_button_active(GuiObject * but, int active);

GuiObject *add_image(GuiWindow * win, int x, int y, char **data);
void show_image(GuiObject *obj);

GuiObject *add_browser(GuiWindow * win, int x, int y, int width, int height, int sliders);
void set_browser_text(GuiObject *obj, char *lines);
void show_browser(GuiObject *obj);

GuiObject *add_pulldown(GuiWindow * win, int x, int y, char *label);
void add_pd_separator(GuiObject * pulldown);
void create_pulldown(GuiObject * pulldown);

GuiObject *add_item(GuiObject * popup, char *label, int type);
void disable_item(GuiObject *obj, int disable);
void show_item(GuiObject *obj);

GuiObject *add_listbox(GuiWindow * win, int x, int y, int width, int nr_lines);
GuiObject *add_listentry(GuiObject * listbox, char *label);
void create_listbox(GuiObject * listbox);
void show_listentry(GuiObject *obj);

void create_iconlist(GuiObject * iconlist);
GuiObject *add_iconlist(GuiWindow * win, int x, int y, int width, int nr_lines);

GuiObject *add_choice(GuiWindow * win, int x, int y, int width);
void create_choice(GuiObject * choice);

GuiObject *add_slider(GuiWindow * win, int type, int x, int y, int length, int buttons);
void set_slider_position(GuiObject * slid, int position);
void set_slider_barlength(GuiObject * slid, int length);
void set_slider_maxvalue(GuiObject * slid, float max);
void show_slider(GuiObject * slid);

GuiObject *add_input(GuiWindow * win, int type, int x, int y, int width, int nr_char);
void set_input(GuiObject * input, char *s);
void show_input(GuiObject * input);

GuiObject *add_text(GuiWindow * win, int type, int x, int y, char *label);
void show_text(GuiObject * text);
void set_text(GuiObject * text, char * s);
void set_text_active(GuiObject * text, int active);
int string_length(char * s);

GuiObject *add_number(GuiWindow * win, int type, int x, int y, int seg_length, char *mask);
void show_number(GuiObject * number);

void print_object_string(GuiObject *obj);
void delete_object(GuiObject * obj);
void hide_object(GuiObject * obj, int hide);
void update_object(GuiObject * obj);
void create_object(GuiObject * obj);
void update_objects(GuiWindow * win);
void show_object(GuiObject *obj);
void reposition_object(GuiObject * obj, int rel_x, int rel_y);
void set_object_active(GuiObject * obj, int active);
void set_object_pressed(GuiObject * obj, int pressed);
void set_object_callback(GuiObject * obj, void (*function) (struct GuiObject_ * obj, int parameter));
void set_object_callback2(GuiObject * obj, void (*function) (struct GuiObject_ * obj, int parameter));	// second callback func.
void set_object_init(GuiObject * obj, void (*function) (struct GuiObject_ * obj));
void set_object_color(GuiObject * obj, int bg1, int bg2, int fg);
void set_object_image_data(GuiObject * obj, char **data, int x_off, int y_off,
			   int pressed, int bg_col);
void set_object_image(GuiObject * obj, char **data);
void set_object_info(GuiObject * obj, char *s);
void set_object_user_data(GuiObject * obj, int data);
void set_object_align(GuiObject * obj, int align);

GuiWindow *add_window(GuiWinThread * win_thread, int type, int x, int y,
		   int width, int height, char *title, int kill_button,
		   int always_on_top);
void activate_window(GuiWindow *win, int active);
void set_window_function(GuiWindow *win, void (*function) (void) );
void reset_window_function(GuiWindow *win);
void delete_window(GuiWindow * win, int update);
void create_window(GuiWindow * win);
void set_maximum_update_region(GuiWindow * win);
void show_window(GuiWindow * win);
void hide_window(GuiWindow * win, int update);
void unhide_window(GuiWindow * win, int update);
void disable_all_windows_except(GuiWindow * win);
void enable_all_windows(GuiWindow * win);

void save_background(GuiWinThread * win_thread);
void set_auto_raise(GuiWinThread * win_thread, int auto_raise);
GuiWinThread * create_window_thread(void);
void delete_window_thread(GuiWinThread * win_thread);
void show_window_thread(GuiWinThread * win_thread);
void do_window_functions(GuiWinThread * win_thread);
void bring_window_to_top(GuiWindow *win);
GuiObject *do_windows(GuiWinThread * win_thread);

GuiWindow *init_progress(GuiWinThread *win_thread, char *title);
void show_progress(GuiWindow *win, float percent);
void delete_progress(GuiWindow *win);

int question_dialog(GuiWinThread *win_thread, GuiWindow *p_win, char *message, int symbol);
void message_dialog(GuiWinThread *win_thread, GuiWindow *p_win, char *message, int symbol);

GuiObject **create_directory_list(char *dirname, int files, GuiObject *List);
GuiWindow *create_file_dialog(GuiWinThread *win_thread, char *title, char *ok_button_str);
int file_dialog(char *directory, char *filename, char *mask);

void enable_info(GuiWinThread *win_thread, int enable);

void set_sleep_time(int time);

void set_update_region(GuiWindow * win, int x_min, int y_min, int x_max, int y_max);
void win_line(GuiWindow * win, int x1, int y1, int x2, int y2, int color, int update_region);
void win_3dline(GuiWindow * win, int x1, int y1, int x2, int y2);
void win_box(GuiWindow * win, int x, int y, int width, int height, int color);
void win_fillbox(GuiWindow * win, int x, int y, int width, int height, int color);
void win_3dbox(GuiWindow * win, int type, int x, int y, int width, int height);
void win_pixmap(GuiObject * obj, int x_pos, int y_pos, int element, int active);
void win_object(GuiObject * obj);

// Special functions.
void init_svgagui(void);
int GuiGetMessage(void);
int GuiMouseGetButton(void);
char GuiKeyboardGetChar(void);

void update_screen(void);
void copy_window_to_screen_image(GuiWindow *win, int savescreen_, int update_mouse, int show_other);

void enable_clipping(void);
void disable_clipping(void);
void set_clipping_window(int x1, int y1, int x2, int y2);
int determine_clip(int *x1, int *y1, int *x2, int *y2);

int check_window(GuiWindow *win, char *function);
int check_object(GuiObject *obj, char *label, char *function);
