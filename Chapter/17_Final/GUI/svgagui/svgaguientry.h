/************************************************************************
	main.h -- last change: 2-1-2007

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

#include "svgagui.h"

extern GuiWindow *color_win;
extern GuiObject *color[3], *color_text;
extern int kleur;

void load_cb(GuiObject * obj, int data);
void save_cb(GuiObject * obj, int data);
void default_cb(GuiObject * obj, int data);
void create_color_win(GuiWinThread * win_thread);
int StartSampleGui();

