/*************************************************************************
	check.c -- last change: 20-1-1998

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

#include <windef.h>
#include <stdio.h>
#include "local.h"

int check_window(GuiWindow *win, char *function)
{
	if (win == NULL) {
		//fprintf(stderr, "---> Warning: win = NULL in %s().\n", function);
		return TRUE;
	}
	
	return FALSE;
}


int check_object(GuiObject *obj, char *label, char *function)
{
	if (obj == NULL) {
		//fprintf(stderr, "---> Warning: %s = NULL in %s().\n", label, function);
		return TRUE;
	}
	
	return FALSE;
}
