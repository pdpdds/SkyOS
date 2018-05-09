/*************************************************************************
	filedialog.c -- last change: 7-12-1998

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
#include "windef.h"
#include "PIT.h"
#include "local.h"
#include "sprintf.h"
//#include "unistd.h"

static GuiWindow *filewin = NULL;
static GuiObject *ok, *cancel, *dir_name, *file_name;
static GuiObject *dir_list, *file_list, *file_type;
static GuiObject **dir_obj, **file_obj;
static int type_nr = 0;
static char filemask[40], file_type_list[10][20];
static int double_click;
static int create_lists(void);
static int wild_match(const char *string, char *pattern);


static void reread_dir(void)
{
	char hulp[256], hulp2[256];// , abs_path[256];
	
	check_window(filewin, "reread_dir");

	sprintf(hulp, "%s", dir_name->label);
//GUI CHECK
	//if (chdir(hulp) == -1) {
	if (1) {
		sprintf(hulp2, "Cannot change to directory\n%s.", hulp);
		message_dialog(filewin->win_thread, filewin, hulp2, DIA_STOP);
//GUI CHECK
		//getcwd(abs_path, 256);
		sprintf(hulp, "%s/", "AAAA");
		set_input(dir_name, hulp);
		show_input(dir_name);
		return;
	}
	/* delete directory objects */
	for (int i = 0; i < dir_list->tot_nr_items; i++)
		delete_object(dir_obj[i]);
	free(dir_obj);
	dir_list->tot_nr_items = 0;

	/* delete file objects */
	for (int i = 0; i < file_list->tot_nr_items; i++)
		delete_object(file_obj[i]);
	free(file_obj);
	file_list->tot_nr_items = 0;

	/* read the new directory */
	create_lists();
	/* update the directory path */
	//getcwd(abs_path, 256);
	sprintf(hulp, "%s/", "AAAA");
	set_input(dir_name, hulp);
	show_input(dir_name);
	update_objects(filewin);
}

#if 0
static void filter_cb(GuiObject * obj, int data)
{
	int i;
	char hulp[256];

	check_object(obj, "obj", "filter_cb");
	check_window(obj->win, "filter_cb");

	for (i = 0; i < dir_list->tot_nr_items; i++)
		if (dir_obj[i]->pressed) {
			sprintf(hulp, "%s%s", dir_name->label, dir_obj[i]->label);
			set_input(dir_name, hulp);
			show_input(dir_name);
		}
	reread_dir();
	show_button(obj);
}
#endif

static void reread_dir_cb(GuiObject * obj, int data)
{
	GuiWindow *win;
	char hulp[256];

	check_object(obj, "obj", "reread_dir_cb");
	win = obj->win;
	check_window(win, "reread_dir_cb");

	if (obj->objclass == LISTENTRY) {
		sprintf(hulp, "%s%s", dir_name->label, obj->label);
		set_input(dir_name, hulp);
		show_input(dir_name);
	}
	reread_dir();
	show_window(win);
}


static void set_file_cb(GuiObject * obj, int data)
{
	check_object(obj, "obj", "set_file_cb");

	set_input(file_name, obj->label);
	show_input(file_name);
}


static void exit_dialog_cb(GuiObject * obj, int data)
{
	check_object(obj, "obj", "exit_dialog_cb");

	double_click = TRUE;
}


#if 0
static int select_files(const struct dirent *dp)
{
	struct stat sbuf;

	stat(dp->d_name, &sbuf);
	if (dp->d_name[0] == '.')
		return 0;
	if (wild_match(dp->d_name, filemask))
		return !S_ISDIR(sbuf.st_mode);
	else
		return 0;
}

static int select_dirs(const struct dirent *dp)
{
	struct stat sbuf;

	stat(dp->d_name, &sbuf);
	if (dp->d_name[0] == '.')
		return 0;
	return S_ISDIR(sbuf.st_mode);
}
#endif

GuiObject **create_directory_list(char *dirname, int files, GuiObject * List)
{
	GuiObject **Items;
	int nr = 0, extra = 0;
	struct dirent **namelist=NULL;

	if (files)
	#if 0
		nr = scandir(dirname, &namelist, select_files, alphasort);
	else {
		nr = scandir(dirname, &namelist, select_dirs, alphasort);
		extra = 2;
	}
	#endif
	nr=-1;
	if (nr < 0)
		return NULL;

	Items = (GuiObject **) malloc(sizeof(GuiObject *) * (nr + extra));
	if (Items == NULL)
		error("Cannot allocate Items[] in create_directory_lists().");
	if (!files) {
		Items[0] = add_listentry(List, ".");
		Items[1] = add_listentry(List, "..");
	}
//gui check
	//for (i = 0; i < nr; i++)
		//Items[i + extra] = add_listentry(List, namelist[i]->d_name);

	create_listbox(List);

	return Items;
}


static int create_lists(void)
{
	int i;

	dir_obj = create_directory_list(".", FALSE, dir_list);
	if (dir_obj == NULL)
		return FALSE;
	for (i = 0; i < dir_list->tot_nr_items; i++)
		set_object_callback2(dir_obj[i], reread_dir_cb);

	file_obj = create_directory_list(".", TRUE, file_list);
	if (file_obj == NULL)
		return FALSE;
	for (i = 0; i < file_list->tot_nr_items; i++) {
		set_object_callback(file_obj[i], set_file_cb);
		set_object_callback2(file_obj[i], exit_dialog_cb);
	}

	return TRUE;
}


static void file_type_cb(GuiObject *obj, int data)
{
	strcpy(filemask, file_type_list[data]);
	reread_dir();
}


static void add_file_type(char *file_type_string, char *file_mask)
{
	GuiObject *obj;

	if (type_nr > 9)
		return;
	
	obj = add_item(file_type, file_type_string, NORMAL_ITEM);
	set_object_callback(obj, file_type_cb);
	set_object_user_data(obj, type_nr);
	strcpy(file_type_list[type_nr], file_mask);
	type_nr++;
}


GuiWindow *create_file_dialog(GuiWinThread * win_thread, char *title, char *ok_button_str)
{
	int width = 380, height = 270;
	int x = (guiscreen.width - width) / 2;
	int y = (guiscreen.height - height) / 2;

	filewin = add_window(win_thread, NORMAL_WINDOW, x, y, width, height,
			 title, TRUE, TRUE);

	check_window(filewin, "init_file_interface");

	y = 60;
	add_text(filewin, NORMAL_TEXT, 10, 26, "Directory:");
	dir_name = add_input(filewin, NORMAL_INPUT, 70, 25, filewin->width - 85, 256);
	filewin->userobj1 = dir_name;
	set_object_callback(dir_name, reread_dir_cb);
	add_text(filewin, NORMAL_TEXT, 10, filewin->height - 49, "Filename:");
	file_name = add_input(filewin, NORMAL_INPUT, 80, filewin->height - 50, 150, 256);
	filewin->userobj2 = file_name;
	add_text(filewin, NORMAL_TEXT, 10, filewin->height - 24, "File of type:");
	type_nr = 0;
	file_type = add_choice(filewin, 80, filewin->height - 25, 140);

	add_text(filewin, NORMAL_TEXT, 10, y, "Directories:");
	dir_list = add_listbox(filewin, 10, y + 15, 120, 9);
	add_text(filewin, NORMAL_TEXT, 155, y, "Files:");
	file_list = add_listbox(filewin, 155, y + 15, 200, 9);
	create_lists();
	ok = add_button(filewin, NORMAL_BUTTON, filewin->width - 55, filewin->height - 50, 50, 20, ok_button_str);
	cancel = add_button(filewin, NORMAL_BUTTON, filewin->width - 55, filewin->height - 25, 50, 20, "Cancel");
	
	return filewin;
}


int file_dialog(char *directory, char *filename, char *mask)
{
	GuiObject *obj = NULL;
	int ret_value = FALSE;
	char hulp[256], hulp2[256], *line1, *line2;// , abs_path[256];

	if (filewin == NULL) {
//		fprintf(stderr, "Filewindow not initialized.\n");
		return FALSE;
	}
		
	if (strlen(directory) == 0) {
		//getcwd(abs_path, 128);
		sprintf(hulp, "%s/", "AAA");
	} else
		sprintf(hulp, "%s", directory);
//GUI CHECK
	//if (chdir(hulp) == -1) {
	if (1) {
		sprintf(hulp2, "Cannot change to directory\n%s.", hulp);
		message_dialog(filewin->win_thread, NULL, hulp2, DIA_STOP);
		//getcwd(abs_path, 128);
		sprintf(hulp, "%s/", "AAA");
	}
	
	/* Add the file type to the choice */
	strcpy(hulp2, mask);
	line1 = strtok(hulp2, "|");
	while (line1 != NULL) {
		line2 = strtok(NULL, "|");
		add_file_type(line1, line2);
		line1 = strtok(NULL, "|");
	}
	create_choice(file_type);
	create_window(filewin);

	set_input(dir_name, hulp);
	show_input(dir_name);
	strcpy(filemask, file_type_list[0]);
	reread_dir();

	double_click = FALSE;
	disable_all_windows_except(filewin);
	activate_window(file_type->win_link, TRUE);
	while (obj != ok && obj != cancel && obj != filewin->kill && !double_click)
		obj = do_windows(filewin->win_thread);
	enable_all_windows(filewin);

	if (obj == ok || double_click) {
		ret_value = TRUE;
		sprintf(filename, "%s", file_name->label);
	}
	sprintf(directory, "%s", dir_name->label);
	delete_window(filewin, TRUE);
	filewin = NULL;

	return ret_value;
}


/* wildmatch.c - Unix-style command line wildcards

   This procedure is in the public domain.

   After that, it is just as if the operating system had expanded the
   arguments, except that they are not sorted.	The program name and all
   arguments that are expanded from wildcards are lowercased.

   Syntax for wildcards:
   *		Matches zero or more of any character (except a '.' at
		the beginning of a name).
   ?		Matches any single character.
   [r3z]	Matches 'r', '3', or 'z'.
   [a-d]	Matches a single character in the range 'a' through 'd'.
   [!a-d]	Matches any single character except a character in the
		range 'a' through 'd'.

   The period between the filename root and its extension need not be
   given explicitly.  Thus, the pattern `a*e' will match 'abacus.exe'
   and 'axyz.e' as well as 'apple'.  Comparisons are not case sensitive.

   The wild_match code was written by Rich Salz, rsalz@bbn.com,
   posted to net.sources in November, 1986.

   The code connecting the two is by Mike Slomin, bellcore!lcuxa!mike2,
   posted to comp.sys.ibm.pc in November, 1988.

   Major performance enhancements and bug fixes, and source cleanup,
   by David MacKenzie, djm@ai.mit.edu. */

/* Shell-style pattern matching for ?, \, [], and * characters.
   I'm putting this replacement in the public domain.

   Written by Rich $alz, mirror!rs, Wed Nov 26 19:03:17 EST 1986. */

/* The character that inverts a character class; '!' or '^'. */
#define INVERT '!'


static int star(const char *string, char *pattern)
{
	while (wild_match(string, pattern) == 0)
		if (*++string == '\0')
			return 0;
	return 1;
}

/* Return nonzero if `string' matches Unix-style wildcard pattern
   `pattern'; zero if not. */

int wild_match(const char *string, char *pattern)
{
	int prev;		/* Previous character in character class. */
	int matched;		/* If 1, character class has been matched. */
	int reverse;		/* If 1, character class is inverted. */

	for (; *pattern; string++, pattern++)
		switch (*pattern) {
		case '\\':
			/* Literal match with following character; fall through. */
			pattern++;
		default:
			if (*string != *pattern)
				return 0;
			continue;
		case '?':
			/* Match anything. */
			if (*string == '\0')
				return 0;
			continue;
		case '*':
			/* Trailing star matches everything. */
			return *++pattern ? star(string, pattern) : 1;
		case '[':
			/* Check for inverse character class. */
			reverse = pattern[1] == INVERT;
			if (reverse)
				pattern++;
			for (prev = 256, matched = 0; *++pattern && *pattern != ']';
			     prev = *pattern)
				if (*pattern == '-'
				? *string <= *++pattern && *string >= prev
				    : *string == *pattern)
					matched = 1;
			if (matched == reverse)
				return 0;
			continue;
		}

	return *string == '\0';
}
