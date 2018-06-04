/*************************************************************************
	bigfont.h -- last change: 20-1-1998

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

 
int bigfont_width[] = {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 	/* !"#$%&'()*+,-./ */
			13,13,13,13,13,13,13,13,13,13,	/* 0123456789 */
			 7, 0, 0, 0, 0, 0, 0,		/* :;<=>?@ */
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* A to Z */
			 0, 0, 0, 0, 0, 0,		/* [\]^_` */
			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* a to z */
			 0, 0, 0};		/* {|} */
int bigfont_height = 18;
char *bigfont[] = {
"                             xxxxxx          xxx       xxxxx        xxxxx            xxx    xxxxxxxxxx      xxxxx    xxxxxxxxxxxx     xxxx        xxxxxx           ",
"                            xxxxxxxx         xxx     xxxxxxxxx    xxxxxxxxx         xxxx    xxxxxxxxxx    xxxxxxxxx  xxxxxxxxxxxx   xxxxxxxx    xxxxxxxxx          ",
"                           xxxx  xxxx       xxxx     xxx   xxxx   xxx   xxx         xxxx    xxx          xxxx   xxx          xxxx   xxx  xxx    xxxx  xxxx         ",
"                           xxx    xxx    xxxxxxx    xxx     xxx  xxx     xxx       xxxxx    xxx          xxx     xxx         xxx   xxx    xxx  xxxx    xxx         ",
"                           xxx    xxx    xxxxxxx    xxx      xxx xxx     xxx       xxxxx    xxx         xxx      xxx        xxx    xxx    xxx  xxx      xxx        ",
"                          xxx      xxx       xxx    xxx      xxx xxx     xxx      xx xxx    xxx         xxx                 xxx    xxx    xxx  xxx      xxx   xxx  ",
"                          xxx      xxx       xxx             xxx         xxx     xxx xxx    xxxxxxxx    xxx                xxx     xxx    xxx  xxx      xxx   xxx  ",
"                          xxx      xxx       xxx            xxx         xxx      xx  xxx    xxxxxxxxx   xxx xxxx           xxx      xxx  xxx   xxx      xxx   xxx  ",
"                xxxxxxx   xxx      xxx       xxx           xxxx      xxxxx      xx   xxx    xxx   xxxx  xxxxxxxxxx        xxx        xxxxxx    xxxx    xxxx        ",
"                xxxxxxx   xxx      xxx       xxx          xxxx       xxxxxxx   xxx   xxx           xxx  xxxxx  xxxx       xxx       xxxxxxxx    xxxx  xxxxx        ",
"                          xxx      xxx       xxx         xxxx            xxx   xx    xxx            xxx xxxx    xxx      xxx       xxx    xxx   xxxxxxxxxxx        ",
"                          xxx      xxx       xxx       xxxxx              xxx xxx    xxx            xxx xxx      xxx     xxx      xxx      xxx    xxxxx xxx        ",
"                          xxx      xxx       xxx      xxxx                xxx xxxxxxxxxxxx          xxx xxx      xxx    xxxx      xxx      xxx          xxx   xxx  ",
"                           xxx    xxx        xxx     xxxx        xxx      xxx xxxxxxxxxxxx xxx      xxx xxx      xxx    xxx       xxx      xxx          xxx   xxx  ",
"                           xxx    xxx        xxx    xxxx         xxx     xxxx        xxx   xxx     xxxx  xxx    xxx     xxx       xxx      xxx xxx     xxx    xxx  ",
"                           xxxx  xxxx        xxx    xxx           xxx   xxxx         xxx   xxxx   xxxx   xxxx  xxxx    xxxx        xxx    xxx  xxxx  xxxxx         ",
"                            xxxxxxxx         xxx    xxxxxxxxxxxx  xxxxxxxxxx         xxx    xxxxxxxxx     xxxxxxxx     xxx         xxxxxxxxxx   xxxxxxxxx          ",
"                             xxxxxx          xxx    xxxxxxxxxxxx    xxxxxx           xxx      xxxxxx       xxxxxx      xxx           xxxxxx       xxxxx            "
};
