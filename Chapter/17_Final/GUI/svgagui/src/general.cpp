/*************************************************************************
	general.c -- last change: 20-1-1998

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

#include "local.h"
#include <stdio.h>

int svgagui_initialized = FALSE, sleep_time = 1;


void set_sleep_time(int time)
{
	sleep_time = time;
}


void init_svgagui(void)
{
	guiscreen.data = NULL;
	#if 0
	guiscreen.image_data = NULL;
	#endif
	
	mouse.savescreen = NULL;
	mouse.visible = FALSE;
	mouse.num_cursors = 0;
	mouse.cursor_id = -1;	/* no mouse defined yet */
	
	set_sleep_time(0);
	svgagui_initialized = TRUE;
}


int GuiGetMessage(void)
{
	int message = FALSE;
	
	switch (guiscreen.type) {
		case SVGALIB : 
			message = get_svga_message();
			break;
#if 0
		case XLIB :
			message = get_X_message();
			break;
#endif
	}

	return message;
}


int GuiMouseGetButton(void)
{
	int button = FALSE;
	
	switch (guiscreen.type) {
		case SVGALIB : 
			button = get_svga_mouse_button();
			break;
#if 0			
		case XLIB :
			button = get_X_mouse_button();
			break;
#endif
	}

	return button;
}


char GuiKeyboardGetChar(void)
{
	char ch = '\0';
	
	switch (guiscreen.type) {
		case SVGALIB:
			ch = get_svga_keyboard_char();
			break;
#if 0
		case XLIB :
			ch = get_X_keyboard_char();
			break;
#endif
	}

	return ch;
}
