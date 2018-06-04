/*************************************************************************
	mouse.h -- last change: 1-3-1997

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

#define mouse_width 10
#define mouse_height 17
static unsigned char mouse_bits[] = {
1,  1,  0,  0,  0,  0,  0,  0,  0,  0,
1,255,  1,  0,  0,  0,  0,  0,  0,  0,
1,255,255,  1,  0,  0,  0,  0,  0,  0,
1,255,255,255,  1,  0,  0,  0,  0,  0,
1,255,255,255,255,  1,  0,  0,  0,  0,
1,255,255,255,255,255,  1,  0,  0,  0,
1,255,255,255,255,255,255,  1,  0,  0,
1,255,255,255,255,255,255,255,  1,  0,
1,255,255,255,255,255,255,255,255,  1,
1,255,255,255,255,255,  1,  1,  1,  0,
1,255,255,  1,255,255,  1,  0,  0,  0,
1,255,  1,  1,255,255,  1,  0,  0,  0,
1,255,  1,  0,  1,255,255,  1,  0,  0,
1,  1,  0,  0,  1,255,255,  1,  0,  0,
0,  0,  0,  0,  0,  1,255,255,  1,  0,
0,  0,  0,  0,  0,  1,255,255,  1,  0,
0,  0,  0,  0,  0,  1,  1,  1,  1,  0};

