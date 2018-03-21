/*
** LikeOS Desktop
** Copyright (C) 2007 Nikolaos Rangos
*/

#include <stdint.h>
//#include <paging.h>
//#include <physalloc.h>
#include "stdio.h"
#include <svgagui.h>
#include "SkyAPI.h"
#include "sprintf.h"
#include "string.h"
#include "PIT.h"
#include "ProcessManager.h"

GuiWindow *color_win;
GuiObject *counter, *color[3], *color_text;

static int count = 0;
static int exit_program = FALSE;

//extern directory[1000][25];
extern char print_possibilities;

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
		for (i = 0;i <= 100;i++) {
			ksleep(4000);
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
	message_dialog((obj->win)->win_thread, NULL, "LikeOS", DIA_INFO);
}


static void question_cb(GuiObject * obj, int data)
{
	question_dialog((obj->win)->win_thread, NULL, "Is this a question?\nYes or no!", DIA_QUESTION);
}

static void endtask_cb(GuiObject * obj, int data)
{
	show_button(obj);
	if (question_dialog((obj->win)->win_thread, NULL, "Do you want to end this task?", DIA_QUESTION)) {
			message_dialog((obj->win)->win_thread, NULL, "You cannot end this task!", DIA_INFO);
	}
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
	for (i = 0;i <= 100;i++) {
		ksleep(4000);
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


static GuiWindow * create_networkconf_win(GuiWinThread * win_thread)
{	
	GuiObject *obj, *pd, *popup, *listbox;
	GuiWindow *win;
	
	win = add_window(win_thread, NORMAL_WINDOW, 200, 200, 400, 300, "LikeOS Network Interfaces", FALSE, FALSE);
	obj = add_text(win, NORMAL_TEXT, 15, 25, "Installed network interfaces:");
	listbox = add_listbox(win, 15, 50, 200, 5);
	obj = add_listentry(listbox, "loopback");
	obj = add_listentry(listbox, "ethernet1");
	create_listbox(listbox);
	obj = add_button(win, NORMAL_BUTTON, 250, 50, 100, 20, "Start");
	obj = add_button(win, NORMAL_BUTTON, 250, 75, 100, 20, "Stop");
	
	obj = add_text(win, NORMAL_TEXT, 15, 150, "Description:");
	obj = add_text(win, NORMAL_TEXT, 15, 170, "Status:");
	obj = add_text(win, NORMAL_TEXT, 15, 190, "IP");
	obj = add_text(win, NORMAL_TEXT, 15, 220, "Address:");
	obj = add_text(win, NORMAL_TEXT, 15, 240, "Netmask:");
	obj = add_text(win, NORMAL_TEXT, 15, 260, "Gateway:");
	obj = add_input(win, NORMAL_INPUT, 70, 220, 150, 256);
	
	add_input(win, NORMAL_INPUT, 70, 240, 150, 256);
	add_input(win, NORMAL_INPUT, 70, 260, 150, 256);

	create_window(win);

	return win;
}

static GuiWindow * create_threadsview_win(GuiWinThread * win_thread)
{
	GuiWindow *win;
	GuiObject *obj,*listbox;	
	int k;
			
	win = add_window(win_thread, NORMAL_WINDOW, (1024-300)/2, (768-500)/2, 300, 500, "LikeOS ThreadsView", FALSE, FALSE);
	obj = add_button(win, NORMAL_BUTTON, 10, 30, 80, 17, "Applications");
	obj = add_button(win, NORMAL_BUTTON, 95, 30, 80, 17, "Performance");
	obj = add_button(win, NORMAL_BUTTON, 180, 30, 80, 17, "Networking");
	obj = add_text(win, NORMAL_TEXT, 5, 60, "Taskname");
	listbox = add_listbox(win, 10, 80, 300-35, 25);
	k=0;
	
	Sky::LinkedList* processList = ProcessManager::GetInstance()->GetProcessList();		
	for (int i = 0; i < processList->Count(); i++)
	{
		Process* pProcess = (Process*)processList->Get(i);
		
		k++;
		obj = add_listentry(listbox, pProcess->m_processName);
		set_object_callback(obj, listbox_cb);
		set_object_callback2(obj, listbox_cb2);
		set_object_user_data(obj, k);
	}

	create_listbox(listbox);
	obj = add_button(win, NORMAL_BUTTON, 200, 450, 80, 20, "End Task");
	set_object_callback(obj, endtask_cb);
	create_window(win);	
	
	return win;
}

static GuiWindow *create_desq(GuiWinThread *win_thread)
{
	GuiWindow *win;
	GuiObject *lb,*obj,*pd;
	char buf[2000];
	
/*	struct isofs_FileDescriptor *fDesc;
	fDesc = isofs_GetFile("MENU.LST");*/
	
	//sprintf(buf, "Desq - Volume: %s %s", getVolumeIdentifier(), fDesc->FileIdentifier);
	
	strcpy(buf, "Desq");
	
	win = add_window(win_thread, NORMAL_WINDOW, (1024-640)/2, (768-480)/2, 640, 480, buf, FALSE, FALSE);
	pd = add_pulldown(win, 4, 20, "File");
	  obj = add_item(pd, "New...", NORMAL_ITEM);
	  //set_object_callback(obj, threadsview_cb);
	  obj = add_item(pd, "Delete", NORMAL_ITEM);
	  //set_object_callback(obj, threadsview_cb);
	  obj = add_item(pd, "Rename", NORMAL_ITEM);
	  //set_object_callback(obj, threadsview_cb);
	  obj = add_item(pd, "Properties", NORMAL_ITEM);
	  //set_object_callback(obj, threadsview_cb);
	  obj = add_item(pd, "Close", NORMAL_ITEM);
	  //set_object_callback(obj, threadsview_cb);
	create_pulldown(pd);

	pd = add_pulldown(win, 30, 20, "Edit");
	  obj = add_item(pd, "Cut", NORMAL_ITEM);
	  //set_object_callback(obj, threadsview_cb);
	  obj = add_item(pd, "Copy", NORMAL_ITEM);
	  //set_object_callback(obj, threadsview_cb);
	  obj = add_item(pd, "Paste", NORMAL_ITEM);
	  //set_object_callback(obj, threadsview_cb);
	  obj = add_item(pd, "Select All", NORMAL_ITEM);
	  //set_object_callback(obj, threadsview_cb);
	create_pulldown(pd);			
	
	
	lb = add_iconlist(win, 10, 40, 640-35, 10);

/*	struct __cdiso_directory2 **dir;
	int j;
	
	dir = (struct __cdiso_directory2**)kalloc(100*sizeof(struct __cdiso_directory2**));
	for (j=0;j<100;j++) {
		dir[j] = (struct __cdiso_directory2*)kalloc(sizeof(struct __cdiso_directory2));
		memset(dir[j],0,sizeof(struct __cdiso_directory2));
	}
	
	int k,cnt;
	char buffer[8096];
		
	cnt = cdiso_getdirectory(dir);
	for (k=0;k<cnt;k++) {
		sprintf(buffer, "%s", (char*)dir[k]->Identifier, dir[k]->NumBytesFileData);
		obj = add_listentry_icon(lb, buffer);
	}	
*/		
	
	create_iconlist(lb);
	create_window(win);
	
	return win;	
}

static void terminalinput_cb(GuiObject * obj, int user_data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;
	GuiObject *object;

	win = obj->win;
	object = win->first;
	
//gui check
	/*if (object->objclass == BROWSER) {
		terminal_dispatch(object, strdup(obj->label));
		memset(obj->label, 0, strlen(obj->label));
		return;
	}
	
	while (object->next != NULL) {
		object = object->next;
		if (object->objclass == BROWSER) {
			terminal_dispatch(object, strdup(obj->label));
			memset(obj->label, 0, strlen(obj->label));			
			return;
		}		
	}*/
}

static GuiWindow *create_terminal(GuiWinThread *win_thread)
{
	GuiWindow *win;
	GuiObject *browser, *text;
	
	win = add_window(win_thread, NORMAL_WINDOW, (1024-640)/2, (768-480)/2, 640, 480, "System Terminal", FALSE, FALSE);
	browser = add_browser(win, 5, 20, 630, 430, VERT_SLIDER);
	set_browser_text(browser, "LikeOS System Terminal initialized.\n");
	text = add_input(win, NORMAL_INPUT, 5, 480-20, 625, 256);
	set_object_callback(text, terminalinput_cb);
	create_window(win);
	
	return win;	
}

extern void portscan();

static void portscan_start_cb(GuiObject * obj, int data)
{
	show_button(obj);
//GUI CHECK
	//portscan();
}

static GuiWindow * create_portscan_win(GuiWinThread * win_thread)
{
	GuiWindow *win;
	GuiObject *obj;
			
	win = add_window(win_thread, NORMAL_WINDOW, (1024-300)/2, (768-500)/2, 500, 500, "LikeOS PortScan (FAST)", FALSE, FALSE);
	obj = add_input(win, NORMAL_INPUT, 65, 60, 160, 25);
	obj = add_text(win, NORMAL_TEXT, 5, 60, "START IP:");
	obj = add_button(win, NORMAL_BUTTON, 200, 450, 160, 20, "START SCANNING");
	set_object_callback(obj, portscan_start_cb);
	create_window(win);	
	
	return win;
}

static void portscan_cb(GuiObject * obj, int user_data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;

	show_button(obj);
		
	win = create_portscan_win(win_thread);
	show_window(win);
}

static void networkconf_cb(GuiObject * obj, int user_data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;

	show_button(obj);
	win = create_networkconf_win(win_thread);
	show_window(win);
}

static void threadsview_cb(GuiObject * obj, int user_data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;

	show_button(obj);
	win = create_threadsview_win(win_thread);
	show_window(win);
}

static void desq_cb(GuiObject * obj, int user_data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;

	show_button(obj);
	win = create_desq(win_thread);
	show_window(win);
}

static void terminal_cb(GuiObject * obj, int user_data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;

	show_button(obj);
	win = create_terminal(win_thread);
	show_window(win);
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
	/*
	sprintf(mask, "C files|*.c|Object files|*.o|All files|*|");
	directory[0] = '\0';
	create_file_dialog((obj->win)->win_thread, "File dialog", "Open");
	if (file_dialog(directory, filename, mask))
		fprintf(stderr, "Dir: %s\nFile: %s\n", directory, filename);*/
}

static void init_interface(GuiWinThread * win_thread)
{
	GuiObject *obj, *pd, *popup, *listbox;
	GuiWindow *win;
	
	/* create the button bar */
	win = add_window(win_thread, NO_TITLE_WINDOW, 0, 0, guiscreen.width, 25, "", FALSE, FALSE);

	pd = add_pulldown(win, 4, 2, "LikeOS");
	  obj = add_item(pd, "PortScan", NORMAL_ITEM);
	  set_object_callback(obj, portscan_cb);		
	  obj = add_item(pd, "Network Conf", NORMAL_ITEM);
	  set_object_callback(obj, networkconf_cb);	
	  obj = add_item(pd, "ThreadsView", NORMAL_ITEM);
	  set_object_callback(obj, threadsview_cb);
	  obj = add_item(pd, "Desq", NORMAL_ITEM);
	  set_object_callback(obj, desq_cb);
	  obj = add_item(pd, "System Terminal", NORMAL_ITEM);
	  set_object_callback(obj, terminal_cb);
	create_pulldown(pd);
	pd = add_pulldown(win, 112, 2, "Information");
	  add_item(pd, "Information index", NORMAL_ITEM);
	  obj = add_item(pd, "About", NORMAL_ITEM);
	  set_object_callback(obj, about_cb);
	create_pulldown(pd);
	create_window(win);
	
/*	win = add_window(win_thread, NORMAL_WINDOW, 100, 100, 360, 265, "LikeOS MoviePlayer", TRUE, FALSE);
	obj = add_movie(win, 0, 0, "Movie1");
	create_window(win);
	create_desq(win_thread);*/
}

#include "Graphics.h"

GuiWinThread *win_thread;


void run_svgagui() {
	GuiObject *obj = NULL;
	int type = SVGALIB;

	
	init_svgagui();
	open_screen(type, 1024, 768, 256, "SVGAGui");
	
	init_mouse();
	int kleur = BACKGROUND;
	win_thread = create_window_thread();
	init_interface(win_thread);
	show_window_thread(win_thread);	

	//rect32B(100, 100, 100, 100, 0x00FF0000, 1024, 768, 24);

	while(1) {
		obj = do_windows(win_thread);
		if (obj == (obj->win)->kill) {
			delete_window(obj->win, FALSE);
		}
	}
	
	delete_window_thread(win_thread);
	close_screen();
}
