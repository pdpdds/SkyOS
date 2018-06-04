/*************************************************************************
	font2.h -- last change: 26-5-1997

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
 
int font_width[] = {4, 2, 4, 7, 6,10, 8, 2, 4, 4, 4, 7, 3, 4, 3, 5, 	/* !"#$%&'()*+,-./ */
		    6, 6, 6, 6, 6, 6, 6, 6, 6, 6,	/* 0123456789 */
		    3, 4, 6, 7, 6, 7, 11,		/* :;<=>?@ */
		    8, 7, 7, 7, 6, 6, 8, 7, 3, 5, 7, 6, 8, 7, 8, 6, 8, 7, 7, 6, 7, 8,12, 7, 8, 7,	/* A to Z */
		    3, 5, 3, 6, 6, 4,		/* [\]^_` */
		    6, 6, 6, 6, 6, 4, 6, 6, 3, 3, 5, 3, 8, 6, 6, 6, 6, 4, 6, 4, 6, 6,10, 6, 6, 6,	/* a to z */
		    4, 3, 4};		/* {|} */
int font_height = 10;
char *font[] = {
"    x x x    x x  xxx   xx   x      xx   x   x x    x                      x  xxx     x   xxx   xxx     x   xxxx  xxx  xxxxx  xxx   xxx                              xxx     xxxxx      x    xxxxx    xxx  xxxx   xxxxx xxxxx   xxx   x    x x     x x    x x     x     x x    x   xxx   xxxx    xxx   xxxxx   xxxx  xxxxx x    x x     x x    x    x x    x x     x xxxxxx xx x    xx    x        x         x               x         x       x     x   x x    x                                             x                                            x x  x   ",
"    x x x    x x x x x x  x x      x  x  x  x   x  xxx                     x x   x   xx  x   x x   x   xx   x    x   x    x  x   x x   x            x        x      x   x  xx     x    x x   x    x  x   x x   x  x     x      x   x  x    x x     x x   x  x     xx   xx xx   x  x   x  x   x  x   x  x    x x    x   x   x    x x     x x   x x   x  x  x   x   x       x x  x     x   x x        x        x               x        x        x           x    x                                             x                                           x  x   x  ",
"    x x x  xxxxx x x   x  x x      x  x  x x     x  x     x               x  x   x  x x      x     x  x x  x     x        x  x   x x   x x   x     x          x         x  x  xx x x   x x   x    x x      x    x x     x     x     x x    x x     x x  x   x     xx   xx x x  x x     x x   x x     x x    x x        x   x    x  x   x   x  x x  x   x  x   x   x      x  x   x    x   x x            xxx  xxxx   xxx   xxxx  xxx  xxx  xxxx xxxx  x   x x  x x  xxx xx  xxxx   xxx  xxxx   xxxx x x  xxx  xxx x   x x   x x   x   x x   x x   x xxxxx  x  x   x  ",
"    x       x x   xxx   xx x        xx     x     x x x    x               x  x   x    x      x   xx   x x  xxxx  xxxx    x    xxx  x   x          x    xxxxx   x       x  x  x  xx x  x   x  xxxxxx x      x    x xxxxx xxxx  x       xxxxxx x     x x xx   x     x x x x x x  x x     x x   x x     x xxxxx   xx      x   x    x  x   x   x  x x  x    xx     x x      x   x   x    x  x   x          x   x x   x x   x x   x x   x  x  x   x x   x x   x x x  x  x  x  x x   x x   x x   x x   x xx  x   x  x  x   x x   x x  x x  x  x x  x   x     x  x  x   x  ",
"    x       x x    x x     x xx    x x     x     x      xxxxx    xxx     x   x   x    x     x      x x  x      x x   x   x   x   x  xxxx         x              x     x   x x   x  x  x   x  x    x x      x    x x     x     x   xxx x    x x     x xx x   x     x x x x x  x x x     x xxxx  x     x x  x      xx    x   x    x   x x    x x   x x    xx      x      x    x    x   x                  xxxx x   x x     x   x xxxxx  x  x   x x   x x   x xx   x  x  x  x x   x x   x x   x x   x x    xx    x  x   x  x x   x x x x    x    x x     x  x   x    x ",
"    x      xxxxx   x x    x x  x  x   xx   x     x        x              x   x   x    x    x       x xxxxx     x x   x  x    x   x     x          x    xxxxx   x      x   x x   x  x  xxxxx  x    x x      x    x x     x     x     x x    x x  x  x x   x  x     x x x x x  x x x     x x     x  xx x x   x       x   x   x    x   x x    x x   x x   x  x     x     x     x    x   x                 x   x x   x x     x   x x      x  x   x x   x x   x x x  x  x  x  x x   x x   x x   x x   x x      x   x  x   x  x x   x x x x    x    x x    x    x  x   x  ",
"           x x   x x x    x x  x  x   x    x     x        x             x    x   x    x   x    x   x    x  x   x x   x  x    x   x x   x           x          x           x x  xx x  x     x x    x  x   x x   x  x     x      x   x  x    x x  x  x x   x  x     x  x  x x   xx  x   x  x      x   x  x   x  x    x   x   x    x    x      x     x    x  x     x    x      x     x  x                 x   x x   x x   x x   x x   x  x  x   x x   x x   x x x  x  x  x  x x   x x   x x   x x   x x   x   x  x  x   x   x     x   x    x x    x    x     x  x   x  ",
"    x      x x    xxx    x   xx    xxx x   x     x            xx      x x     xxx     x  xxxxx  xxx     x   xxx   xxx   x     xxx   xxx  x   x      x        x        x   x  xx xx   x     x xxxxx    xxx  xxxx   xxxxx x       xxx   x    x x   xx  x    x xxxxx x  x  x x    x   xxx   x       xxx x x    x  xxxx    x    xxxx     x      x     x   x    x    x    xxxxxx x     x  x                  xxxx xxxx   xxx   xxxx  xxx   x   xxxx x   x x   x x  x x  x  x  x x   x  xxx  xxxx   xxxx x    xxx   xx  xxxx   x     x   x   x   x   x   xxxxx  x  x   x  ",
"                   x                        x   x              x                                                                             x                             x       x                                                                                                                                                                                        x        x                                                       x           x                             x         x                                             x          x  x   x  ",
"                                             x x               x                                                                             x                              xxxxxxx                                                                                                                                                                                         xx      xx       xxxxxx                                      xxxx           x                              x         x                                            x            x x  x   "};
