/*************************************************************************
	svgalib.c -- last change: 19-11-2000

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
//#include <stdlib.h>
#include "windef.h"
//#include "bootinfo.h"
#include "memory.h"
#include "vesa.h"
#include <string.h>
//#include <mouse.h>
//#include <keyb.h>

static char savechar = '\0';
static unsigned long color[50];

//32bit!!
void update_svga_screen(void)
{
	int length, line, x;
	char *src;
	unsigned long *dst32;
	unsigned long *buffer = getlfb();
		
	src = guiscreen.data + guiscreen.x_min + guiscreen.y_min * guiscreen.width;
	length = guiscreen.x_max - guiscreen.x_min + 1;
	dst32 = buffer + guiscreen.x_min + guiscreen.y_min * guiscreen.width;
	
	for (line = 0; line < guiscreen.y_max - guiscreen.y_min + 1;line++) {
		for (x = 0;x < length;x++)
			*(dst32++) = *(color + *src++);
		dst32 += guiscreen.width - length;
		src += guiscreen.width - length;
	}
}

//24ºñÆ®
/*void update_svga_screen(void)
{
	int length, line, x;
	char *src;
	char *dst32;
	unsigned long *lfb = getlfb();

	src = guiscreen.data;
	length = guiscreen.x_max - guiscreen.x_min + 1;
	dst32 = (char*)lfb;
	
	for (line = 0; line < guiscreen.y_max - guiscreen.y_min + 1; line++)
	{
		for (x = 0; x < length; x++)
		{
			int index = ((x + guiscreen.x_min) + (guiscreen.y_min + line) * guiscreen.width) * 3;
			int srcIndex = ((x + guiscreen.x_min) + (guiscreen.y_min + line) * guiscreen.width);
			dst32[index] = (char)(*(color + src[srcIndex]) >> 0);
			index++;
			dst32[index] = (char)(*(color + src[srcIndex]) >> 8);
			index++;
			dst32[index] = (char)(*(color + src[srcIndex]) >> 16);
		}		
	}
}*/

void init_svga_mouse()
{
	/*
	if (mouse_init("/dev/mouse", vga_getmousetype(), MOUSE_DEFAULTSAMPLERATE))
		error("Could not initialize mouse.");

	mouse_setdefaulteventhandler();
	mouse_setxrange(0, guiscreen.width - 1);
	mouse_setyrange(0, guiscreen.height - 1);
	mouse_setposition(guiscreen.width / 2, guiscreen.height / 2);
    */
	//mouse.x = mouse_getx();
	//mouse.y = mouse_gety();
}


void get_svga_mouse_position(void)
{
	//mouse.x = mouse_getx();
	//mouse.y = mouse_gety();
}


void set_svga_mouse_position(int x, int y)
{
	//mouse_setposition(x, y);
}


int get_svga_message(void)
{
//gui check
	/*if ((savechar = vga_getkey()))
		return GuiKeyboardEvent;

	if (mouse_update())
		return GuiMouseEvent;*/
			
	return FALSE;
}


int get_svga_mouse_button(void)
{
	int status = 0;
//gui check	
	/*status = mouse_getbutton();
	if (status & 1) {
		if (status & 2)
			return GuiMouseMiddleButton;
		else
			return GuiMouseLeftButton;
	}
	if (status == 3)
		return GuiMouseMiddleButton;
	if (status == 2)
		return GuiMouseRightButton;*/
		
	return FALSE;
}


char get_svga_keyboard_char(void)
{
	return savechar;
}


void save_svga_screen_to_xpm(void)
{
	//FILE *fd;
	//int x, y, i, count, r, g, b, nr_colors;
	//int buffer[256];
#if 0
	/* determine the number of colors used */
	nr_colors = 0;
	for (i = 0; i < guiscreen.width * guiscreen.height; i++) {
		count = 0;
		while (buffer[count] != guiscreen.data[i] && count < nr_colors)
			count++;
		if (count == nr_colors) {	/* color not found */
			nr_colors++;
			buffer[nr_colors] = guiscreen.data[i];
		}
	}

	//fd = fopen("screen.xpm", "w");
	fd = NULL;
	if (fd == NULL)
		error("Cannot open screen.xpm for writing in save_screen_to_xpm().");

	fprintf(fd, "/* XPM */\n");
	fprintf(fd, "static char * screen_xpm[] = {\n");
	fprintf(fd, "\"%d %d %d 1\",\n", guiscreen.width, guiscreen.height, nr_colors);
	for (i = 0; i < nr_colors; i++) {
		gl_getpalettecolor(color[i], &r, &g, &b);
		r = (int) (r / 64.0 * 255);
		g = (int) (g / 64.0 * 255);
		b = (int) (b / 64.0 * 255);
		fprintf(fd, "\"%c   c #%02x%02x%02x\",\n", i + 65, r, g, b);
	}

	for (y = 0; y < guiscreen.height; y++) {
		fprintf(fd, "\"");
		for (x = 0; x < guiscreen.width; x++) {
			i = 0;
			while (guiscreen.data[x + guiscreen.width * y] != color[i])
				i++;
			fprintf(fd, "%c", i + 65);
		}
		fprintf(fd, "\",\n");
	}
	fclose(fd);
#endif	
}

static void define_32bit_colors(void)
{
	/* Define the extra colors */
	BLACK      = 1;
	DARKGREY   = 2;
	GREY       = 3;
	LIGHTGREY  = 4;
	WHITE = 5;
	DARKRED    = 6;
	RED        = 7;
	DARKBLUE   = 8;
	BLUE       = 9;
	LIGHTBLUE  = 10;
	DARKGREEN  = 11;
	GREEN      = 12;
	DARKYELLOW = 13;
	YELLOW     = 14;

	/* redefine the colorscheme */
	BACKGROUND        = 0;
	WIN_BACK          = 15;
	TITLE_BACK        = DARKGREY;
	TITLE_FORE        = GREY;
	ACTIVE_TITLE_BACK = DARKBLUE;
	ACTIVE_TITLE_FORE = WHITE;
	BUTTON_BACK       = GREY;
	BUTTON_FORE       = BLACK;
	SLIDER_BACK       = LIGHTGREY;
	SLIDER_FORE       = LIGHTGREY;
	TEXT_EMBOSS       = DARKGREY;
	TEXT_NORMAL       = BLACK;
	INPUT_BACK        = GREY;
	INPUT_FORE        = BLACK;
	ACTIVE_INPUT_BACK = LIGHTGREY;
	BROWSER_BACK      = WHITE;
	BROWSER_FORE      = BLACK;
	INFO_BACK         = 16;
	INFO_FORE         = BLACK;
	CHOICE_BACK       = WHITE;
	CHOICE_FORE       = BLACK;
	LISTBOX_BACK      = WHITE;
	LISTBOX_FORE      = BLACK;
	NUMBER_FORE       = 17;

	color[BACKGROUND]  = 0x608189;
	color[WIN_BACK]    = 0xC0C0C0;
	color[INFO_BACK]   = 0xDCDCA8;
	color[NUMBER_FORE] = 0x4880B8;

	color[BLACK]      = 0x000000;
	color[DARKGREY]   = 0x909090;
	color[GREY]       = 0xC0C0C0;
	color[LIGHTGREY]  = 0xD8D8D8;
	color[WHITE]      = 0xFFFFFF;
	color[DARKRED]    = 0x780000;
	color[RED]        = 0xE80000;
	color[DARKBLUE]   = 0x808080;
	color[BLUE]       = 0x0000C0;
	color[LIGHTBLUE]  = 0x0080F0;
	color[DARKGREEN]  = 0x007800;
	color[GREEN]      = 0x54FF54;
	color[DARKYELLOW] = 0xFFBC00;
	color[YELLOW]     = 0xFFFF54;
}


void set_svga_default_palette(int colors)
{
}

void open_svga_screen(int width, int height, int colors)
{
/*	int vgamode = -1;
	char hulp[80];

	vga_init();
	switch (colors) {
	case 16:
		if (width == 320 && height == 200)
			vgamode = G320x200x16;
		if (width == 640 && height == 200)
			vgamode = G640x200x16;
		if (width == 640 && height == 350)
			vgamode = G640x350x16;
		if (width == 640 && height == 480)
			vgamode = G640x480x16;
		break;
	case 256:
		if (width == 320 && height == 200)
			vgamode = G320x200x256;
		if (width == 640 && height == 480)
			vgamode = G640x480x256;
		if (width == 800 && height == 600)
			vgamode = G800x600x256;
		if (width == 1024 && height == 768)
			vgamode = G1024x768x256;
		break;
	default:
		error("Wrong number of colors specified (16 or 256)");
	}

	if (!vga_hasmode(vgamode)) {
		sprintf(hulp, "Mode %dx%dx%d not available", width, height, colors);
		error(hulp);
	}

	vga_setmode(vgamode);
	if (colors > 16) {
		gl_setcontextvga(vgamode);
		define_256_colors();
	} else
		define_16_colors();
	set_svga_default_palette(colors);*/

	/* Allocate memory for the screen image */
	define_32bit_colors();
	if (guiscreen.data)
		free(guiscreen.data);
	guiscreen.data = (char *) malloc(width * height);
	if (guiscreen.data == NULL)
		error("Cannot allocate memory for screen operations in open_screen().");
			
	memset(guiscreen.data, 0, width * height);
	guiscreen.type = SVGALIB;
}


void close_svga_screen(void)
{
	/*if (guiscreen.type == SVGALIB)
		vga_setmode(TEXT);*/
}


void set_svga_palette(int col, int red, int green, int blue)
{
//	gl_setpalettecolor(color, red, green, blue);
}


void get_svga_palette(int col, int *red, int *green, int *blue)
{
//	gl_getpalettecolor(color, red, green, blue);
}


void switch_svga_cursor(int cursor_id)
{

}