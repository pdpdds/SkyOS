/************************************************************************
	main.c -- last change: 2-1-2007

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
#include "play.xpm"
#include "sprintf.h"
#include "SkyAPI.h"
#include "fileio.h"
#include "string.h"

#define VERSION		"1.2"

/*void error(char *s)
{
	close_screen();
	//fprintf(g_s, "Error: %s\n", s);
	//exit(0);
	for (;;);
}*/

GuiWindow *color_win;
GuiObject *counter, *color[3], *color_text;
int kleur, symbol = 0;

static int count = 0;
static int exit_program = FALSE;

void counter_cb(GuiObject * obj, int data)
{
	GuiWindow *win = counter->win;
	int min, hour;

	if (data == 0)		/* decrease */
		count--;
	if (data == 1)		/* increase */
		count++;

	hour = count / 60;
	min = count % 60;
	sprintf(counter->label, "%02d:%02d", hour, min);
	win->x_min = 0;		/* update full window */
	win->y_min = 0;
	win->x_max = win->width - 1;
	win->y_max = win->height - 1;
	show_number(counter);
	if (data == 0)
		show_button(obj);
}


void all_dialogs_cb(GuiObject * obj, int data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;
	int i;

	if (question_dialog(win_thread, NULL, "Show progress dialog box ?", DIA_QUESTION)) {
		message_dialog(win_thread, NULL, "Press OK to start.", FALSE);
		win = init_progress(win_thread, "Testing 1,2,3....");
		for (i = 0; i <= 100; i++) {
			//usleep(4000);
			show_progress(win, i / 100.0);
		}
		delete_progress(win);
	}
	if (obj->objclass == BUTTON)
		show_button(obj);
}


static GuiWindow * create_slider_win(GuiWinThread * win_thread)
{
	GuiWindow *win;
	GuiObject *obj;

	win = add_window(win_thread, NORMAL_WINDOW, 170, 50, 155, 170, "Sliders", TRUE, FALSE);
	obj = add_slider(win, NICE_VALUE_SLIDER, 20, 40, 101, FALSE);
	set_slider_maxvalue(obj, 1.0);
	obj = add_slider(win, NICE_VALUE_SLIDER, 70, 40, 101, FALSE);
	set_slider_maxvalue(obj, 10);
	obj = add_slider(win, NICE_VALUE_SLIDER, 120, 40, 101, FALSE);
	set_slider_maxvalue(obj, 100);
	obj = add_slider(win, NICE_HOR_SLIDER, 10, 150, 135, FALSE);
	create_window(win);

	return win;
}


static void window_cb(GuiObject * obj, int user_data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;

	show_button(obj);
	win = create_slider_win(win_thread);
	show_window(win);
}


static void button_cb(GuiObject * obj, int data)
{
	show_button(obj);
}


static void about_cb(GuiObject * obj, int data)
{
	message_dialog((obj->win)->win_thread, NULL, "This is svgagui version " VERSION "\nCopyright 2007\nby B. Nagels", DIA_INFO);
}


static void question_cb(GuiObject * obj, int data)
{
	question_dialog((obj->win)->win_thread, NULL, "Is this a question?\nYes or no!", DIA_QUESTION);
}


static void message_cb(GuiObject * obj, int data)
{
	message_dialog((obj->win)->win_thread, NULL, "This is a message consisting\nof two lines", data);
}


void progress_cb(GuiObject * obj, int data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;
	int i;

	win = init_progress(win_thread, "Testing 1,2,3....");
	for (i = 0; i <= 100; i++) {
		//usleep(4000);
		show_progress(win, i / 100.0);
	}
	delete_progress(win);
}


static void exit_cb(GuiObject * obj, int data)
{
	exit_program = TRUE;
	//fprintf(stderr, "Goodbye.\n");
}


static void listbox_cb(GuiObject * obj, int data)
{
	//fprintf(stderr, "Listbox: Testing %d pressed\n", data);
}


static void listbox_cb2(GuiObject * obj, int data)
{
	//fprintf(stderr, "Listbox: Testing %d double clicked\n", data);
}


static void autoraise_cb(GuiObject * obj, int data)
{
	((obj->win)->win_thread)->auto_raise = obj->pressed;
}


static void enablewin_cb(GuiObject * obj, int data)
{
	if (obj->pressed)
		enable_all_windows(obj->win);
	else
		disable_all_windows_except(obj->win);
}


static void file_cb(GuiObject * obj, int data)
{
	char directory[128], filename[128], mask[20];

	sprintf(mask, "C files|*.c|Object files|*.o|All files|*|");
	directory[0] = '\0';
	create_file_dialog((obj->win)->win_thread, "File dialog", "Open");
	if (file_dialog(directory, filename, mask))
	{

	}
		//fprintf(stderr, "Dir: %s\nFile: %s\n", directory, filename);
}

static void init_interface(GuiWinThread * win_thread)
{
	GuiObject *obj, *pd, *popup, *listbox;
	GuiWindow *win;
	char text[] = "This is a test of the browser function.\n\
Using the sliders you are able to read\n\
this text. Scrolling up and down and left and right.\n\
This\n\
is\n\
a\n\
test.\n\
Testing 1...2";

	/* create the button bar */
	win = add_window(win_thread, NO_TITLE_WINDOW, 0, 0, guiscreen.width, 42, "", FALSE, FALSE);

	pd = add_pulldown(win, 4, 2, "File");
	obj = add_item(pd, "Open", NORMAL_ITEM);
	set_object_callback(obj, file_cb);
	popup = add_item(pd, "Import", POPUP_ITEM);
	add_item(popup, "Test 1", NORMAL_ITEM);
	/*	    add_item(popup, "Test 2", NORMAL_ITEM);
	add_item(popup, "Test 3", NORMAL_ITEM); */
	create_pulldown(popup);
	popup = add_item(pd, "Export", POPUP_ITEM);
	obj = add_item(popup, "Inactive", NORMAL_ITEM);
	set_object_active(obj, FALSE);
	add_item(popup, "Check 1", CHECK_ITEM);
	add_item(popup, "Check 2", CHECK_ITEM);
	create_pulldown(popup);
	add_pd_separator(pd);
	add_item(pd, "Check1", CHECK_ITEM);
	obj = add_item(pd, "Check2", CHECK_ITEM);
	set_object_pressed(obj, TRUE);
	obj = add_item(pd, "This is a test", NORMAL_ITEM);
	set_object_active(obj, FALSE);
	add_pd_separator(pd);
	obj = add_item(pd, "Exit", NORMAL_ITEM);
	set_object_callback(obj, exit_cb);
	create_pulldown(pd);
	pd = add_pulldown(win, 35, 2, "Color");
	obj = add_item(pd, "Load settings", NORMAL_ITEM);
	set_object_callback(obj, load_cb);
	obj = add_item(pd, "Save settings", NORMAL_ITEM);
	set_object_callback(obj, save_cb);
	obj = add_item(pd, "Default colors", NORMAL_ITEM);
	set_object_callback(obj, default_cb);
	create_pulldown(pd);
	pd = add_pulldown(win, 71, 2, "Other");
	obj = add_item(pd, "Show question dialog", NORMAL_ITEM);
	set_object_callback(obj, question_cb);
	popup = add_item(pd, "Show message dialog", POPUP_ITEM);
	obj = add_item(popup, "Question", NORMAL_ITEM);
	set_object_callback(obj, message_cb);
	set_object_user_data(obj, DIA_QUESTION);
	obj = add_item(popup, "Exclamation", NORMAL_ITEM);
	set_object_callback(obj, message_cb);
	set_object_user_data(obj, DIA_EXCLAMATION);
	obj = add_item(popup, "Stop", NORMAL_ITEM);
	set_object_callback(obj, message_cb);
	set_object_user_data(obj, DIA_STOP);
	obj = add_item(popup, "Info", NORMAL_ITEM);
	set_object_callback(obj, message_cb);
	set_object_user_data(obj, DIA_INFO);
	create_pulldown(popup);
	obj = add_item(pd, "Show progress dialog", NORMAL_ITEM);
	set_object_callback(obj, progress_cb);
	create_pulldown(pd);
	pd = add_pulldown(win, 112, 2, "Help");
	add_item(pd, "Help index", NORMAL_ITEM);
	obj = add_item(pd, "About", NORMAL_ITEM);
	set_object_callback(obj, about_cb);
	create_pulldown(pd);

	obj = add_button(win, PIXMAP_BUTTON, 300, 20, 44, 20, "");
	set_object_image(obj, play_xpm);
	set_object_info(obj, "Info testje");
	set_object_callback(obj, button_cb);
	obj = add_button(win, NORMAL_BUTTON, 350, 20, 44, 20, "OK");
	set_object_callback(obj, all_dialogs_cb);
	set_object_color(obj, DARKGREEN, RED, YELLOW);
	set_object_info(obj, "Show all dialogs");
	win->enterobj = obj;
	obj = add_button(win, NORMAL_BUTTON, 405, 20, 40, 20, "Down");
	set_object_callback(obj, counter_cb);
	set_object_user_data(obj, 0);
	set_object_info(obj, "Decrease counter");
	obj = add_button(win, NORMAL_BUTTON, 445, 20, 40, 20, "Up");
	set_object_callback(obj, counter_cb);
	set_object_user_data(obj, 1);
	set_object_info(obj, "Increase counter");
	obj->wait_for_mouse = FALSE;
	obj = add_button(win, NORMAL_BUTTON, guiscreen.width - 50, 20, 44, 20, "Quit");
	set_object_info(obj, "Quit");
	set_object_callback(obj, exit_cb);
	add_button(win, LIGHT_BUTTON, 246, 20, 50, 20, "Light");
	obj = add_button(win, LIGHT_BUTTON, 192, 20, 50, 20, "Light");
	set_object_color(obj, DARKGREEN, RED, WHITE);
	obj = add_button(win, NORMAL_BUTTON, 5, 20, 100, 20, "Slider Window");
	set_object_callback(obj, window_cb);
	obj = add_button(win, CHECK_BUTTON, 110, 23, 0, 0, "Auto raise");
	set_object_callback(obj, autoraise_cb);
	set_object_pressed(obj, TRUE);
	obj = add_button(win, CHECK_BUTTON, guiscreen.width - 140, 23, 0, 0, "Enable win");
	set_object_callback(obj, enablewin_cb);
	set_object_pressed(obj, TRUE);
	create_window(win);
	win_3dline(win, 2, 16, guiscreen.width - 4, 16);

	win = add_window(win_thread, NORMAL_WINDOW, 0, 50, 150, 125, "Special buttons", FALSE, FALSE);
	obj = add_button(win, CHECK_BUTTON, 5, 23, 0, 0, "Check 1");
	obj->pressed = TRUE;
	add_button(win, CHECK_BUTTON, 5, 43, 0, 0, "Check 2");
	add_button(win, CHECK_BUTTON, 5, 63, 0, 0, "Check 3");
	add_button(win, RADIO_BUTTON, 5, 83, 0, 0, "Radio 1");
	add_button(win, RADIO_BUTTON, 5, 103, 0, 0, "Radio 2");
	create_window(win);

	win = add_window(win_thread, NORMAL_WINDOW, 0, 230, 250, 100, "Plain text (always on top)", FALSE, TRUE);
	obj = add_text(win, NORMAL_TEXT, 5, 25, "The Quick Fox Jumps Over The Lazy Dog.");
	set_object_color(obj, WIN_BACK, BLACK, WHITE);
	set_object_info(obj, "With a label!");
	obj = add_text(win, NORMAL_TEXT, 5, 38, "This is some red text.");
	set_object_color(obj, WIN_BACK, BLACK, RED);
	add_text(win, NORMAL_TEXT, 5, 60, "This is normal text.");
	add_text(win, EMBOSSED_TEXT, 5, 73, "This is embossed text.");
	create_window(win);

	create_color_win(win_thread);
	create_slider_win(win_thread);


	win = add_window(win_thread, NORMAL_WINDOW, 180, 340, 150, 107, "Browser", FALSE, FALSE);
	obj = add_browser(win, 5, 21, 127, 68, TRUE);
	set_browser_text(obj, text);
	create_window(win);

	win = add_window(win_thread, NORMAL_WINDOW, 0, 340, 165, 130, "Different inputs", FALSE, FALSE);
	add_text(win, NORMAL_TEXT, 5, 25, "Normal");
	add_input(win, NORMAL_INPUT, 55, 25, 100, 256);
	add_text(win, NORMAL_TEXT, 5, 47, "Float");
	add_input(win, FLOAT_INPUT, 55, 47, 100, 256);
	add_text(win, NORMAL_TEXT, 5, 69, "Integer");
	add_input(win, INT_INPUT, 55, 69, 100, 256);
	counter = add_number(win, NORMAL_NUMBER, 15, 98, 10, "00:00");
	create_window(win);
	win_3dline(win, 3, 90, 160, 90);
	win_fillbox(win, 5, 95, 90, 30, BLACK);
	update_object(counter);

	win = add_window(win_thread, NORMAL_WINDOW, 340, 340, 250, 105, "Colored text", FALSE, FALSE);
	obj = add_text(win, NORMAL_TEXT, 5, 25, "black");
	set_object_color(obj, WIN_BACK, BLACK, BLACK);
	obj = add_text(win, NORMAL_TEXT, 5, 40, "darkgrey");
	set_object_color(obj, WIN_BACK, BLACK, DARKGREY);
	obj = add_text(win, NORMAL_TEXT, 5, 55, "grey");
	set_object_color(obj, WIN_BACK, BLACK, GREY);
	obj = add_text(win, NORMAL_TEXT, 5, 70, "lightgrey");
	set_object_color(obj, WIN_BACK, BLACK, LIGHTGREY);
	obj = add_text(win, NORMAL_TEXT, 5, 85, "white");
	set_object_color(obj, WIN_BACK, BLACK, WHITE);
	obj = add_text(win, NORMAL_TEXT, 80, 25, "darkred");
	set_object_color(obj, WIN_BACK, BLACK, DARKRED);
	obj = add_text(win, NORMAL_TEXT, 80, 40, "red");
	set_object_color(obj, WIN_BACK, BLACK, RED);
	obj = add_text(win, NORMAL_TEXT, 80, 55, "darkblue");
	set_object_color(obj, WIN_BACK, BLACK, DARKBLUE);
	obj = add_text(win, NORMAL_TEXT, 80, 70, "blue");
	set_object_color(obj, WIN_BACK, BLACK, BLUE);
	obj = add_text(win, NORMAL_TEXT, 80, 85, "lightblue");
	set_object_color(obj, WIN_BACK, BLACK, LIGHTBLUE);
	obj = add_text(win, NORMAL_TEXT, 160, 25, "darkgreen");
	set_object_color(obj, WIN_BACK, BLACK, DARKGREEN);
	obj = add_text(win, NORMAL_TEXT, 160, 40, "green");
	set_object_color(obj, WIN_BACK, BLACK, GREEN);
	obj = add_text(win, NORMAL_TEXT, 160, 55, "darkyellow");
	set_object_color(obj, WIN_BACK, BLACK, DARKYELLOW);
	obj = add_text(win, NORMAL_TEXT, 160, 70, "yellow");
	set_object_color(obj, WIN_BACK, BLACK, YELLOW);
	create_window(win);

	win = add_window(win_thread, NORMAL_WINDOW, 340, 220, 200, 105, "Listbox", FALSE, FALSE);
	listbox = add_listbox(win, 5, 25, 70, 2);
	obj = add_listentry(listbox, "Testing 1");
	set_object_callback(obj, listbox_cb);
	set_object_callback2(obj, listbox_cb2);
	set_object_user_data(obj, 1);
	obj = add_listentry(listbox, "Testing 2");
	set_object_callback(obj, listbox_cb);
	set_object_callback2(obj, listbox_cb2);
	set_object_user_data(obj, 2);
	obj = add_listentry(listbox, "Testing 3");
	set_object_callback(obj, listbox_cb);
	set_object_callback2(obj, listbox_cb2);
	set_object_user_data(obj, 3);
	obj = add_listentry(listbox, "Testing 4");
	set_object_callback(obj, listbox_cb);
	set_object_callback2(obj, listbox_cb2);
	set_object_user_data(obj, 4);
	obj = add_listentry(listbox, "Testing 5");
	set_object_callback(obj, listbox_cb);
	set_object_callback2(obj, listbox_cb2);
	set_object_user_data(obj, 5);
	obj = add_listentry(listbox, "Testing 6");
	set_object_callback(obj, listbox_cb);
	set_object_callback2(obj, listbox_cb2);
	set_object_user_data(obj, 6);
	create_listbox(listbox);
	create_window(win);
}


int StartSampleGui()
{
	GuiWinThread *win_thread;
	GuiObject *obj = NULL;
	int type = SVGALIB;

	//GUI 모드 초기화
	init_svgagui();
	
	//1024*768 32비트로 초기화한다. 
	open_screen(type, 1024, 768, 256, "SVGAGui");
	
	//마우스 이미지만 초기화
	init_mouse();
	kleur = BACKGROUND;

	//윈도우 컴포넌트를 담을 수 있는 윈도우 쉬트를 생성한다.
	win_thread = create_window_thread();

	//샘플 윈도우 컴포넌트를 윈도우 쉬트에 추가한다.
	init_interface(win_thread);	
	
	//갱신된 버퍼를 화면에 출력한다.
	show_window_thread(win_thread);
	
	//입력 처리가 구현되어 있지 않으므로 루프에서 벗어나지 않는다.
	while (!exit_program) {
		obj = do_windows(win_thread);
		if (obj == (obj->win)->kill)
			delete_window(obj->win, TRUE);
	}

	delete_window_thread(win_thread);
	close_screen();

	return 0;
}


