/*$
  $ LikeOS GUI Subsystem
  $ Copyright (C) 2006 Nikolaos Rangos, Microtrends Ltd.
  $
  $*/

#define __CDELF__

#include "windef.h"
#include "vesa.h"
#include "svga_window.h"
#include "string.h"
#include "font.h"

unsigned char *wallpaper;
unsigned char *windowicon;
unsigned char *closeicon;
unsigned char *maxicon;
unsigned char *minicon;
unsigned char *combobox1;
unsigned char *scrollt;
unsigned char *scrollb;
unsigned char *scrollm;
unsigned char *scrollbt;
unsigned char *scrollbm;
unsigned char *scrollbb;
struct WND * wnd_head = nullptr;

guiroot root;
shell Shell;
int windowz[MAXWINDOWS][2];

unsigned long *offscreen;
char did=0;

#define SCREENWIDTH 1024
#define SCREENHEIGHT 768

int mouse_isinside(int x, int y, int x2, int y2, int width, int height) {
		if ((x >= x2) && (x <= x2 + width) && (y <= (y2+height)) && (y >= y2))
			return 1;
			else
			return 0;	
}

void guiroot::init() {
	//wallpaper=(unsigned char*) kalloc(cdiso_getfilesize("LIKEOS.RAW"));
	//cdiso_readfile("LIKEOS.RAW", wallpaper);
	/*windowicon=(unsigned char*) kmalloc(cdiso_getfilesize("WND1.RAW"));
	cdiso_readfile("WND1.RAW", windowicon);	
	closeicon=(unsigned char*) kmalloc(cdiso_getfilesize("CLOSE.RAW"));
	cdiso_readfile("CLOSE.RAW", closeicon);	
	maxicon=(unsigned char*) kmalloc(cdiso_getfilesize("MAX.RAW"));
	cdiso_readfile("MAX.RAW", maxicon);	
	minicon=(unsigned char*) kmalloc(cdiso_getfilesize("MIN.RAW"));
	cdiso_readfile("MIN.RAW", minicon);	
	combobox1=(unsigned char*) kalloc(cdiso_getfilesize("CBOX1.RAW"));
	cdiso_readfile("CBOX1.RAW", combobox1);	
	scrollt=(unsigned char*) kalloc(cdiso_getfilesize("SCLTOP.RAW"));
	cdiso_readfile("SCLTOP.RAW", scrollt);
	scrollb=(unsigned char*) kalloc(cdiso_getfilesize("SCLBTM.RAW"));
	cdiso_readfile("SCLBTM.RAW", scrollb);
	scrollbt=(unsigned char*) kalloc(cdiso_getfilesize("SCLBT.RAW"));
	cdiso_readfile("SCLBT.RAW", scrollbt);
	scrollbb=(unsigned char*) kalloc(cdiso_getfilesize("SCLBB.RAW"));
	cdiso_readfile("SCLBB.RAW", scrollbb);
	scrollbm=(unsigned char*) kalloc(cdiso_getfilesize("SCLBM.RAW"));
	cdiso_readfile("SCLBM.RAW", scrollbm);
			*/
	offscreen=(unsigned long*) new char[SCREENWIDTH*SCREENHEIGHT*4];
	font_init();
}

void guiroot::DrawHorizLine(int x,
					   int y,
					   int width,
					   unsigned char r,
					   unsigned char g,
					   unsigned char b) {
		int i;

		for (i=0;i<width;i++) {
				if ((x+i>=0) && (x+i<=SCREENWIDTH) && (y>=0) && (y<=SCREENWIDTH))
				offscreen[SCREENWIDTH*y+(x+i)]=(r<<16) | (g<<8) | b;
		}
	}	

void guiroot::DrawDottedHorizLine(int x,
					   int y,
					   int width,
					   int x2,
					   int y2) {
		int i;
		unsigned long *lfb = getlfb();

		for (i=0;i<width;i+=2) {
				if ((x2+i>=0) && (x2+i<=SCREENWIDTH) && (y2>=0) && (y2<=SCREENWIDTH)) {
						lfb[SCREENWIDTH*y2+(x2+i)]=offscreen[SCREENWIDTH*y2+(x2+i)];
				}
		}
				
		for (i=0;i<width;i+=2) {
				if ((x+i>=0) && (x+i<=SCREENWIDTH) && (y>=0) && (y<=SCREENWIDTH))
				lfb[SCREENWIDTH*y+(x+i)]=1;
		}
	}	
	
		
void guiroot::DrawVertiLine(int x,
					   int y,
					   int height,
					   unsigned char r,
					   unsigned char g,
					   unsigned char b) {
		int i;

		for (i=0;i<height;i++) {
				if ((x>=0) && (x<=SCREENWIDTH) && (y+i>=0) && (y+i<=SCREENWIDTH))
				offscreen[SCREENWIDTH*(y+i)+x]=(r<<16) | (g<<8) | b;
		}
	}

void guiroot::DrawDottedVertiLine(int x,
					   int y,
					   int height,
					   int x2,
					   int y2) {
		int i;
		unsigned long *lfb = getlfb();

		for (i=0;i<height;i+=2) {
				if ((x2>=0) && (x2<=SCREENWIDTH) && (y2+i>=0) && (y2+i<=SCREENWIDTH))
					lfb[SCREENWIDTH*(y2+i)+x2]=offscreen[SCREENWIDTH*(y2+i)+x2];
		}		
				
		for (i=0;i<height;i+=2) {
				if ((x>=0) && (x<=SCREENWIDTH) && (y+i>=0) && (y+i<=SCREENWIDTH))
				lfb[SCREENWIDTH*(y+i)+x]=1;
		}
	}	
	
void guiroot::Fillrect( int x,
						int y,
						int width,
						int height,
						unsigned char r,
						unsigned char g,
						unsigned char b)
{
	int x2;
	int y2;
	
	for (y2=0;y2<height;y2++) {
		for (x2=0;x2<width;x2++) {
			if ((x2+x>=0) && (x2+x<=SCREENWIDTH) && (y2+y>=0) && (y2+y<=SCREENWIDTH))
			offscreen[SCREENWIDTH*(y2+y)+(x2+x)]=(r<<16) | (g<<8) | b;	
		}
	}	
}

void guiroot::drawImage(int x1, int y1, int width, int height, unsigned char *buf) {
		unsigned char r,g,b;
		unsigned long i,x,y;
		
		if ((x1<0) || (y1<0)) return;
		i=0;
		for (y=0; y<height; y++) {
			i=width*y*3;
			if (((y+y1) < 0) || ((y+y1)>SCREENWIDTH)) continue;
			for (x=0; x<width; x++) {
				if (((x+x1) < 0) || ((x+x1)>SCREENWIDTH)) continue;
				r=buf[i];
				g=buf[i+1];
				b=buf[i+2];
				i+=3;		
			
				offscreen[SCREENWIDTH*(y+y1)+(x+x1)] = (r<<16) | (g<<8) | b;
			}
		}
}
	
void guiroot::drawWallpaper() {
		unsigned char r,g,b;
		unsigned long i,x;
		
		i=0;
		for (x=0; x<SCREENWIDTH*SCREENHEIGHT; x++) {
			/*r=wallpaper[i];
			**g=wallpaper[i+1];
			**b=wallpaper[i+2];
			*/
			i+=3;		
			
			r=15;
			g=10;
			b=73;
			
			offscreen[x] = (r<<16) | (g<<8) | b;
		}
}

void guiroot::doUpdate() {
		/*$$$*/
		lfb_update(offscreen);
		/*$$$*/
}

void container::Draw(int x2, int y2) {
	root.DrawHorizLine(x2+x,y2+y,width,172,168,153);
	root.DrawHorizLine(x2+x+1,y2+y+1,width-1,255,255,255);
	root.DrawHorizLine(x2+x,y2+y+height,width,172,168,153);
	root.DrawHorizLine(x2+x,y2+y+height+1,width,255,255,255);
	root.DrawVertiLine(x2+x,y2+y,height,172,168,153);
	root.DrawVertiLine(x2+x+1,y2+y+1,height-1,255,255,255);
	root.DrawVertiLine(x2+x+width,y2+y,height,172,168,153);
	root.DrawVertiLine(x2+x+width+1,y2+y+1,height-1,255,255,255);
	
	
	root.DrawHorizLine(x2+x+width/4-10,y2+y,strlen(title)*FONT_WIDTH+25,241,240,227);
	root.DrawHorizLine(x2+x+width/4-10,y2+y+1,strlen(title)*FONT_WIDTH+25,241,240,227);
	putstringc(title,x2+x+width/4,y2+y-7,0,0,0,offscreen);
}

void container::onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown) {}
void container::onRelease(int x, int y, int oldx, int oldy, int leftbuttondown) {}

void container::createcontainer(char *title1, int x1, int y1, int width1, int height1) {
	strcpy(title, title1);
	x=x1;
	y=y1;
	width=width1;
	height=height1;
}

void button::Draw(int x2, int y2)
{
	root.Fillrect(x2+x,y2+y,width,height,212,208,200);
	if (buttondown == 0) {
		root.Fillrect(x2+x,y2+y,width,height,253,252,250);
		root.DrawHorizLine(x2+x+1,y2+y+1,width+1,102,101,84);
		root.DrawHorizLine(x2+x+1,y2+y+height+1,width,102,101,84);
		root.DrawVertiLine(x2+x+1,y2+y+1,height+1,102,101,84);
		root.DrawVertiLine(x2+x+width+1,y2+y+1,height+1,102,101,84);
		root.DrawHorizLine(x2+x,y2+y,width,210,210,198);
		root.DrawVertiLine(x2+x,y2+y,height,210,210,198);
	} else {
		int i;
		for (i=0; i<height; i++) {
			root.DrawHorizLine(x2+x,y2+y+i,width,253-i,252-i,250-i);
		}
		root.DrawHorizLine(x2+x+1,y2+y+1,width+1,102,101,84);
		root.DrawHorizLine(x2+x+1,y2+y+height+1,width,102,101,84);
		root.DrawVertiLine(x2+x+1,y2+y+1,height+1,102,101,84);
		root.DrawVertiLine(x2+x+width+1,y2+y+1,height+1,102,101,84);
		root.DrawHorizLine(x2+x,y2+y,width,210,210,198);
		root.DrawVertiLine(x2+x,y2+y,height,210,210,198);		
	}
//		putstringc(title,x2+x+width/2-(strlen(title)*FONT_WIDTH)/2,y2+y+4,0,0,0,offscreen);
}

void button::createbutton(char *title1, int x1, int y1, int width1, int height1) {
	strcpy(title, title1);
	x=x1;y=y1;width=width1;height=height1;buttondown=0;
}

void button::onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown) {
	buttondown=1;
	Shell.handleWithFocus=handle;
	refresh=1;
}

void button::onRelease(int x, int y, int oldx, int oldy, int leftbuttondown) {
	buttondown=0;
	refresh=1;
}

void textbox::onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown) {
	Shell.handleWithFocus=handle;
	refresh=1;
}

void control::Draw(int x, int y){}
void control::onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown) {}
void control::onRelease(int x, int y, int oldx, int oldy, int leftbuttondown) {}
void control::onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown) {}
void control::onMouseMove(int x, int y, int oldx, int oldy, int leftbuttondown) {}
void control::onKeyPressed(char *s) {}

control::control()
{
	x=0;y=0;width=0;height=0;widthoffset=0;heightoffset=0;
}

void label::Draw(int x2, int y2) {
//	putstringc(message, x2+x, y2+y, 0, 0, 0, offscreen);
}

void label::createlabel(char *message1, int x1, int y1) {
	strcpy(message, message1);	
	x=x1;
	y=y1;
}

void textbox::createtextbox(char *initialtext, int x1, int y1, int width1, int height1) {
	strcpy(text, initialtext);
	x=x1;
	y=y1;
	width=width1;
	height=height1;
	drawOffset=0;
}

void textbox::Draw(int x2, int y2) {
	
	root.Fillrect(x+x2,y+y2,width,height,255,255,255);
	root.DrawHorizLine(x+x2+1,y+y2+1,width+1,102,101,84);
	root.DrawHorizLine(x+x2+1,y+y2+height+1,width+1,102,101,84);
	root.DrawVertiLine(x+x2+1,y+y2+1,height+1,102,101,84);
	root.DrawVertiLine(x+x2+width+1,y+y2+1,height+1,102,101,84);
	root.DrawHorizLine(x+x2,y+y2,width,210,210,198);
	root.DrawVertiLine(x+x2,y+y2,height,210,210,198);
	
//	putstringc(&text[drawOffset],x2+x+2,y2+y+4,0,0,0,offscreen);
	if (strlen(text)*FONT_WIDTH+12 > width) {
		root.DrawVertiLine(x+x2+2+width-5, y+y2+3, 16, 0, 0, 0);
	} else
		root.DrawVertiLine(x+x2+2+strlen(text)*FONT_WIDTH+1, y+y2+3, 16, 0, 0, 0);
}

void textbox::onKeyPressed(char *s) {
	if ((s[0] == 8) && (strlen(text) != 0)) {
		text[strlen(text)-1]='\0';
		if (drawOffset!=0) drawOffset--;
	} else {
		if (strlen(text)*FONT_WIDTH+12 > width) drawOffset++;
		if ((strlen(text)*FONT_WIDTH+12 < width) && (drawOffset != 0)) drawOffset=0;
	}
	
	if ((s[0] != 8)) strcat(text, s);
	refresh=1;
}

void combobox::additem(char *item) {
	strcpy(items[numItems], item);
	numItems++;	
}

void combobox::createcombobox(char *item, int x1, int y1, int width1, int height1) {
	strcpy(topitem, item);
	strcpy(items[0], item);
	topItemID=0;
	numItems=1;
	x=x1;
	y=y1;
	width=width1;
	height=height1;
}

void combobox::Draw(int x2, int y2) {
	root.Fillrect(x+x2,y+y2,width,height,255,255,255);
	root.DrawHorizLine(x+x2+1,y+y2+1,width+1,102,101,84);
	root.DrawHorizLine(x+x2+1,y+y2+height+1,width+1,102,101,84);
	root.DrawVertiLine(x+x2+1,y+y2+1,height+1,102,101,84);
	root.DrawVertiLine(x+x2+width+1,y+y2+1,height+1,102,101,84);
	root.DrawHorizLine(x+x2,y+y2,width,210,210,198);
	root.DrawVertiLine(x+x2,y+y2,height,210,210,198);	
	//root.drawImage(x+x2+width-18,y+y2+3,18,18,combobox1);
		
	putstringc(topitem,x2+x+2,y2+y+4,0,0,0,offscreen);
	
	if (isOpened == 1) {
		int i,boxheight;
		height2 = numItems*FONT_HEIGHT+4,i;
		width2 = width;
		boxheight = height2;
		root.Fillrect(x+x2+1, y+y2+3+height, width, boxheight, 255, 255, 255);
		root.Fillrect(x+x2+1, y+y2+3+height+(selectedItem*FONT_HEIGHT), width, FONT_HEIGHT+2, 249, 224, 137);		
		root.DrawHorizLine(x+x2, y+y2+3+height, width, 102, 101, 84);
		root.DrawHorizLine(x+x2, y+y2+3+height+boxheight+1, width+1, 102, 101, 84);
		root.DrawVertiLine(x+x2, y+y2+3+height, boxheight+1, 102, 101, 84);
		root.DrawVertiLine(x+x2+width, y+y2+3+height, boxheight+1, 102, 101, 84);
		for (i=1;i<4;i++) {
			root.DrawHorizLine(x+x2+i, y+y2+3+height+boxheight+1+i, width, 102+i*25, 101+i*25, 84+i*25);
			root.DrawVertiLine(x+x2+width+i, y+y2+3+height+i, boxheight+1, 102+i*25, 101+i*25, 84+i*25);
		}
		
		for (i=0;i<numItems;i++) {
			putstringc(items[i], x2+x+2, y2+y+height+(i*FONT_HEIGHT)+4, 0, 0, 0, offscreen);
		}
	}
}

void listbox::addItem(char *name) {
	strcpy(items[numItems++], name);
}

void listbox::createlistbox(char *item, int x1, int y1, int width1, int height1) {	
	numItems = 0;
	x=x1;
	y=y1;
	width=width1;
	height=height1;
	selectedItem=0;
	topOffset=0;
	firstItem=0;
}

void listbox::Draw(int x2, int y2) {
	int i;
	
	root.Fillrect(x+x2,y+y2,width,height,255,255,255);
	root.DrawHorizLine(x+x2+1,y+y2+1,width+1,102,101,84);
	root.DrawHorizLine(x+x2+1,y+y2+height+1,width+1,102,101,84);
	root.DrawVertiLine(x+x2+1,y+y2+1,height+1,102,101,84);
	root.DrawVertiLine(x+x2+width+1,y+y2+1,height+1,102,101,84);
	root.DrawHorizLine(x+x2,y+y2,width,210,210,198);
	root.DrawVertiLine(x+x2,y+y2,height,210,210,198);
	
	root.Fillrect(x+x2+2, y+y2+2+(selectedItem*FONT_HEIGHT), width-1, FONT_HEIGHT+2, 249, 224, 137);
	
	if (height < numItems*FONT_HEIGHT) {
		root.Fillrect(x+x2+width-14,y+y2+2,15,height-14,231,228,217);
		root.drawImage(x+x2+width-14,y+y2+2,15,15,scrollt);
		root.drawImage(x+x2+width-14,y+y2+height-14,15,15,scrollb);
		
		root.drawImage(x+x2+width-14,y+y2+2+15+topOffset,15,2,scrollbt);
		int k,m;
		
		//$$$Prozentwert = Grundwert * (Prozentsatz/100)
		//$$$Prozentsatz = Prozentwert / (Grundwert*100)		
		
		m = (numItems*FONT_HEIGHT)/(height/FONT_HEIGHT);	/*$$$*/
		scrollbarSize = m;
		
		for (k=0;k!=m;k++) {
		root.drawImage(x+x2+width-14,y+y2+4+15+k+topOffset,15,1,scrollbm);
		}
		
		root.drawImage(x+x2+width-14,y+y2+4+15+k+topOffset,15,2,scrollbb);
	}
	
	for (i=0;i<numItems;i++) {
		if (height < (i*FONT_HEIGHT)+15) break;
		putstringc(items[i+firstItem], x2+x+2, y2+y+(i*FONT_HEIGHT)+4, 0, 0, 0, offscreen);
	}
}

void listbox::onMouseMove(int x1, int y1, int windowsx, int windowsy, int leftbuttondown) {
}

void listbox::onClick(int x1, int y1, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown) {
	int i,hit=0;
	
	if (mouse_isinside( x1,
						y1,
						windowsx+x+width-14,
						windowsy+y+height-14,
						15,
						15) == 1) {
		if (topOffset+15+scrollbarSize+15+4+height/15 < height) {
			topOffset += height/15;
			firstItem++;
		}
		hit=1;
	}	
	
	if (mouse_isinside( x1,
						y1,
						windowsx+x+width-14,
						windowsy+y+2,
						15,
						15) == 1) {
		if (topOffset > 0)	{
			topOffset -= height/15;
			firstItem--;
		}
		hit=1;
	}	

	if (hit == 1) {
		refresh=1;
		Shell.update();
		return;	
	}
	
	for (i=0;i<numItems;i++) {
			if (mouse_isinside( x1,
						   		y1,
						   		windowsx+x+2,
						   		windowsy+y+(i*FONT_HEIGHT)+4,
						   		width-1,
						   		FONT_HEIGHT+2) == 1) {
				if (selectedItem != i) {
					if ((i*FONT_HEIGHT+15) > height) break;
					selectedItem=i;
					refresh=1;
					Shell.update();
				}
				break;
		}
	}	
}

void listbox::onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown) {
	
}

void menu::addtopmenu() {
	int k;
	numItems1++;
	
	for (k=0; k<numItems1; k++) {
		opened[k]=0;
	}		
}

void menu::additem(char *title, int i, int j) {
	strcpy(items[i][j], title);
	strcpy(items[i][j+1], "$");
}

void menu::createmenu(char *item, int x1, int y1, int width1, int height1) {
	numItems1 = 0;
	x=x1;
	y=y1;
	width=width1;
	height=height1;
}

void menu::Draw(int x2, int y2) {
	int i,x3,y3;
	
	root.Fillrect(x+x2,y+y2, width-12, 22, 240, 240, 226);
	root.DrawHorizLine(x+x2, y+y2+23, width-10, 210, 210, 198);
	root.DrawHorizLine(x+x2, y+y2+24, width-10, 246, 246, 239);
	
	int k=15;
	for (i=0;i<numItems1;i++) {
		int r,g,b;
		r=0;g=0;b=0;
//		putstringc(items[i][0], x2+x+k, y2+y+4, r, g, b, offscreen);
		itemsx[i]=k;
		k+=(FONT_WIDTH*strlen(items[i][0])) + 15;
	}
		
	for (i=0;i<numItems1;i++) {		
		if (opened[i] == 1) {
			for (k=1;;k++) {
				if (items[i][k][0] == '$')
					break;	
			}
			k++;
			
			int r,g,b;
			
			x3=x+x2+itemsx[i];
			y3=y+y2+FONT_HEIGHT+5;
			width2 = 150;
			height2 = k*FONT_HEIGHT;
			widthoffset = x2+itemsx[i];
			heightoffset = y2+FONT_HEIGHT+5;

			root.Fillrect(x3,y3,width2,height2,247,246,239);									
			root.DrawHorizLine(x3,y3,width2,102,101,84);
			root.DrawVertiLine(x3,y3,height2,102,101,84);
			root.DrawHorizLine(x3,y3+height2,width2+1,102,101,84);
			root.DrawVertiLine(x3+width2,y3,height2,102,101,84);
			int m;
			for (m=1;m<4;m++) {
				root.DrawHorizLine(x3+m,y3+height2+m,width2+1,102+m*28,101+m*28,84+m*28);
				root.DrawVertiLine(x3+width2+m,y3+m,height2,102+m*28,101+m*28,84+m*28);
			}
			
			for (k=1;;k++) {
				if (items[i][k][0] == '$') break;
				
				if (mouseOverItem==k) {
					root.Fillrect(x3+1, y3+height+((mouseOverItem-1)*FONT_HEIGHT)-8, width2-1, FONT_HEIGHT+2, 249, 224, 137);
				}
				putstringc(items[i][k], x3+3+5, y3+(FONT_HEIGHT*k), 0, 0, 0, offscreen);
			}
						
			break;
		}
	}
}

void menu::onClick(int x1, int y1, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown) {
	int i,k;

	for (i=0; i<numItems1; i++) {
		if (mouse_isinside(x1,
						   y1,
						   windowsx+itemsx[i]+1,
						   windowsy+19,
						   strlen(items[i][0])*FONT_WIDTH,
						   39) == 1) {
			opened[i] = 1;
			for (k=0; k<numItems1; k++) {
				if (i!=k)
					opened[k]=0;
			}
			
			Shell.updateWindow(windowHandle);
			break;
		}
	}
}

void menu::onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown) {
	int i;
	
	for (i=0; i < numItems1; i++) {
		opened[i]=0;
	}
	
	width2=0;
	height2=0;
	widthoffset=0;
	heightoffset=0;
	
	Shell.updateWindow(windowHandle);
}

void menu::onMouseMove(int x1, int y1, int windowsx, int windowsy, int leftbuttondown) {
	int i,k;
	
	for (i=0;i<numItems1;i++) {
		if (opened[i] == 1) {
			for (k=1;;k++) {
				if (items[i][k][0] == '$') goto end;
				
				if (mouse_isinside( x1,
									y1,
									windowsx+itemsx[i]+1,
									windowsy+19+(FONT_HEIGHT*(k+1))+5,
									width2,
									FONT_HEIGHT ) == 1) {
					
					if (mouseOverItem != k) {
						mouseOverItem = k;
						refresh=1;
						Shell.update();
					}
					goto end;						
				}	
			}
		}	
	}

end:

return;
}

void taskbar::Draw(int x2, int y2) {
	int btnwidth=70;
	int btnheight=25;
	
	x2 = 2;
	y2 = 2;
	
	root.Fillrect(0, SCREENHEIGHT-28, SCREENWIDTH, 28, 238, 237, 223);
	root.DrawHorizLine(0, SCREENHEIGHT-29, SCREENWIDTH, 255, 255, 255);
	root.DrawHorizLine(0, SCREENHEIGHT-30, SCREENWIDTH, 164, 164, 158);
	
	///root.Fillrect(x2+x,y2+y,btnwidth,btnheight,212,208,200);
	
	if (buttondown == 0) {
		root.Fillrect(x2+x,y2+y,btnwidth,btnheight,253,252,250);
		root.DrawHorizLine(x2+x+1,y2+y+1,btnwidth+1,102,101,84);
		root.DrawHorizLine(x2+x+1,y2+y+btnheight+1,btnwidth,102,101,84);
		root.DrawVertiLine(x2+x+1,y2+y+1,btnheight+1,102,101,84);
		root.DrawVertiLine(x2+x+btnwidth+1,y2+y+1,btnheight+1,102,101,84);
		root.DrawHorizLine(x2+x,y2+y,btnwidth,210,210,198);
		root.DrawVertiLine(x2+x,y2+y,btnheight,210,210,198);		
	} else {
		int i;
		for (i=0; i<btnheight; i++) {
			root.DrawHorizLine(x2+x,y2+y+i,btnwidth,253-i*3,252-i*3,250-i*3);
		}
		root.DrawHorizLine(x2+x+1,y2+y+1,btnwidth+1,102,101,84);
		root.DrawHorizLine(x2+x+1,y2+y+btnheight+1,btnwidth,102,101,84);
		root.DrawVertiLine(x2+x+1,y2+y+1,btnheight+1,102,101,84);
		root.DrawVertiLine(x2+x+btnwidth+1,y2+y+1,btnheight+1,102,101,84);
		root.DrawHorizLine(x2+x,y2+y,btnwidth,210,210,198);
		root.DrawVertiLine(x2+x,y2+y,btnheight,210,210,198);		
	}	
	
		putstringc("LikeOS",x2+x+btnwidth/2-(6*FONT_WIDTH)/2,y2+y+4,0,0,0,offscreen);
	
	if (buttondown == 1) {
		width2=253;
		height2=468;
		int x3,y3;
		x3=x2+x;
		y3=y+y2-height2-2;

		root.DrawHorizLine(x3,y3,width2,212,208,200);
		root.DrawHorizLine(x3+1,y3+1,width2-2,255,255,255);
		root.DrawVertiLine(x3,y3,height2-1,212,208,200);
		root.DrawVertiLine(x3+1,y3+1,height2-2,255,255,255);
		root.DrawHorizLine(x3+1,y3+height2-2,width2-1,128,128,128);
		root.DrawHorizLine(x3,y3+height-1,width2,64,64,64);
		root.DrawVertiLine(x3+width2,y3,height2,64,64,64);
		root.DrawVertiLine(x3+width2-1,y3+1,height2-2,128,128,128);
		root.Fillrect(x3+2,y3+2,width2-3,height2-4,241,240,227);
	}
	
	int i,k=200;
	for (i=0,k=0;i<Shell.windowscount;i++) {
		if (Shell.windows[i]->minimized == 1) {
			k+=(strlen(Shell.windows[i]->title)*FONT_WIDTH);
			putstringc(Shell.windows[i]->title,x2+x+((strlen(Shell.windows[i]->title)*FONT_WIDTH)/2)+k,y2+y+4,0,0,0,offscreen);
		}
	}
}

void taskbar::onClick(int x1, int y1, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown) {

	if (mouse_isinside( x1,
						y1,
						x+2,
						y+2,
						70,
						25 ) == 1) {
		if (buttondown == 0) buttondown=1;
	} else {
		if (buttondown == 1) {
			buttondown=0;
			Shell.updateAll();
		}
	}

	refresh=1;		
}

void taskbar::onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown) {
	buttondown = 0;
	refresh=1;
	Shell.updateAll();
}

void taskbar::onMouseMove(int x1, int y1, int windowsx, int windowsy, int leftbuttondown) {
	
}

void taskbar::createtaskbar(char *name, int x1, int y1, int width1, int height1) {
	x=x1;
	y=y1;
	width=width1;
	height=height1;
	buttondown=0;
}

void combobox::onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown) {
	isOpened=0;
	height2=0;
	width2=0;
	Shell.updateWindow(windowHandle);
}

void combobox::onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown) {
	if (isOpened == 0) {
		isOpened=1;
		refresh=1;
		return;
	}
	
	if (isOpened == 1) {
		if (strcmp(topitem, items[selectedItem]) != 0)
			strcpy(topitem, items[selectedItem]);
			topItemID = selectedItem;
			height2=0;
			width2=0;
			isOpened=0;
			Shell.updateWindow(windowHandle);
	}
}

void combobox::onMouseMove(int x1, int y1, int windowsx, int windowsy, int leftbuttondown) {
	int i;
	
	if (isOpened == 1) {
		for (i=0;i<numItems;i++) {
			if (mouse_isinside( x1,
						   		y1,
						   		windowsx+x+2,
						   		windowsy+y+height+(i*FONT_HEIGHT)+4,
						   		width-1,
						   		FONT_HEIGHT+2) == 1) {
				if (selectedItem != i) {
					selectedItem=i;
					refresh=1;
					Shell.update();
				}
				break;
		}
	}   
	}
}

int previousx;
int previousy;

void window::onClick(int x1, int y1, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown) {
	int i;

	
	if (mouse_isinside(	x1,
						y1,
						Shell.windows[Shell.windowWithFocus]->x+width-60,
						Shell.windows[Shell.windowWithFocus]->y,
						16,
						20) == 1) {
			minimized = 1;
			
			for (i=0;i<Shell.windowscount;i++) {
				if (Shell.windows[i]->z == z-1) {
						Shell.windowWithFocus = Shell.windows[i]->handle; 
						break;
					}
			}
			
			Shell.updateAll();
			return;
	}	
		
	if (mouse_isinside(x1,
					   y1,
					   Shell.windows[Shell.windowWithFocus]->x,
					   Shell.windows[Shell.windowWithFocus]->y,
					   Shell.windows[Shell.windowWithFocus]->width,
				   	   Shell.windows[Shell.windowWithFocus]->height) != 1) {
		if (Shell.windowWithFocus != handle) {
			
			for (i=0;i<Shell.windowscount;i++) {
				if (Shell.windows[i]->z >= z) z=Shell.windows[i]->z+1;
			}
			
			Shell.windowWithFocus = handle;
			Shell.updateAll();
		}
		return;
	}
	
	if (Shell.windowWithFocus == handle) {
		if (mouse_isinside(x1,
					   	   y1,
					   	   x,
					   	   y,
					   	   width,
					   	   20) == 1) {
						   	   windowmoving=1;
						   	   previousx=x1;
						   	   previousy=y1;
					   	   }
		return;
	}
}

void window::onRelease(int x1, int y1, int oldx1, int oldy1, int leftbuttondown) {
	if (windowmoving == 1) {
		x=x1-(width/2);
		y=y1;
		windowmoving=0;
		Shell.updateAll();
	}	
}

void window::onMouseMove(int x1, int y1, int reserved1, int reserved2, int leftbuttondown) {
	int rectx,recty,rectx2,recty2;
	if (windowmoving == 1) {
		rectx=x1-(width/2);
		recty=y1-15;
		rectx2=previousx-(width/2);
		recty2=previousy-15;
		
		root.DrawDottedHorizLine(rectx, recty, width, rectx2, recty2);
		root.DrawDottedVertiLine(rectx, recty, height, rectx2, recty2);
		root.DrawDottedHorizLine(rectx, recty+height, width, rectx2, recty2+height);
		root.DrawDottedVertiLine(rectx+width, recty, height, rectx2+width, recty2);
		
		root.DrawDottedHorizLine(rectx+1, recty+1, width, rectx2+1, recty2+1);
		root.DrawDottedVertiLine(rectx+1, recty+1, height, rectx2+1, recty2+1);
		root.DrawDottedHorizLine(rectx+1, recty+height+1, width, rectx2+1, recty2+height+1);
		root.DrawDottedVertiLine(rectx+width+1, recty+1, height, rectx2+width+1, recty2+1);

		root.DrawDottedHorizLine(rectx+2, recty+2, width, rectx2+2, recty2+2);
		root.DrawDottedVertiLine(rectx+2, recty+2, height, rectx2+2, recty2+2);
		root.DrawDottedHorizLine(rectx+2, recty+height+2, width, rectx2+2, recty2+height+2);
		root.DrawDottedVertiLine(rectx+width+2, recty+2, height, rectx2+width+2, recty2+2);		
				
		previousx=x1;
		previousy=y1;
	}
}

void window::createwindow(char *title1, int width1, int height1) {			 	
		     x=SCREENWIDTH/2-width1/2;
			 y=SCREENHEIGHT/2-height1/2;
			 
			 if (invisible == 1) {
				x=0;
				y=0;	 
			 }
			 
			 width=width1;
			 height=height1;
			 windowmoving=0;
			 strcpy(title, title1);
			 minimized=0;
}

void window::drawwindow()
{
	if ((invisible == 0) && (minimized == 0))
	{
		root.Fillrect(x, y, width, height, 241, 240, 227);
		root.DrawHorizLine(x, y, width, 158, 190, 233);
		root.DrawVertiLine(x, y, height, 117, 156, 210);
		root.Fillrect(x + 1, y + 1, width - 1, 20, 95, 143, 209);
		root.Fillrect(x + 1, y + 1, 3, height - 1, 95, 143, 209);
		root.DrawHorizLine(x + 4, y + 21, width - 4, 50, 90, 145);
		root.DrawVertiLine(x + 4, y + 21, height - 21, 50, 90, 145);
		root.DrawVertiLine(x + width, y + 1, height - 1, 51, 91, 146);
		root.Fillrect(x + width - 3, y + 1, 3, height - 1, 95, 143, 209);
		root.DrawVertiLine(x + width - 4, y + 22, height - 22, 115, 154, 207);
		root.DrawHorizLine(x, y + height, width, 50, 90, 145);
		root.Fillrect(x + 1, y + height - 3, width - 1, 3, 67, 120, 193);
		root.DrawHorizLine(x + 4, y + height - 4, width - 8, 115, 154, 207);
		//root.drawImage(x + 8, y + 4, 15, 15, windowicon);
		//root.drawImage(x + width - 24, y + 3, 16, 16, closeicon);
		//root.drawImage(x + width - 42, y + 3, 16, 16, maxicon);
		//root.drawImage(x + width - 60, y + 3, 16, 16, minicon);
		//putstringc(title, x + width / 2 - ((strlen(title)*FONT_WIDTH) / 2), y + 4, 255, 255, 255, offscreen);
	}
}

void shell::updateAll()
{
	int i, k, j, l, temp;

	for (i = 0; i < windowscount; i++) {
		windowz[i][0] = windows[i]->handle;
		windowz[i][1] = windows[i]->z;
	}

	/*$$$ Bubblesort the windows Z $$$*/
	for (;;) {
		l = 0;
		for (i = 0; i < windowscount - 1; i++) {
			for (j = 0; j < windowscount - 1 - i; j++) {
				if (windowz[j][1] > windowz[j + 1][1]) {
					temp = windowz[j][1];
					windowz[j][1] = windowz[j + 1][1];
					windowz[j + 1][1] = temp;
					temp = windowz[j][0];
					windowz[j][0] = windowz[j + 1][0];
					windowz[j + 1][0] = temp;
					l = 1;
				}
			}
		}
		if (l == 0) break;
	}
	/*$$$*/

	//root.drawWallpaper();

	for (i = 0; i < windowscount; i++) 
	{
		if (windows[windowz[i][0]]->minimized != 1) 
		{
			windows[windowz[i][0]]->drawwindow();
			for (k = 0; k < windows[windowz[i][0]]->controlscount; k++) 
			{
				windows[windowz[i][0]]->controls[k]->Draw(windows[windowz[i][0]]->x, windows[windowz[i][0]]->y);
				windows[windowz[i][0]]->controls[k]->refresh = 0;
			}
		}
	}

	root.doUpdate();
	//mouse_draw_mousepointerwoxy();
}
		
void shell::updateWindow(int i) {
	int k;
		if (Shell.windows[i]->minimized != 1) {
			Shell.windows[i]->drawwindow();
			for (k=0;k<windows[i]->controlscount;k++) {
					windows[i]->controls[k]->Draw(windows[i]->x, windows[i]->y);
					windows[i]->controls[k]->refresh=0;
			}
		}
}

void shell::update() {
			int i,k;
			
			for (i=0;i<windowscount;i++) {
				if (i==windowWithFocus) {
					for (k=0;k<windows[i]->controlscount;k++) {
						if (windows[i]->controls[k]->refresh == 1) {
							windows[i]->controls[k]->Draw(windows[i]->x, windows[i]->y);
							windows[i]->controls[k]->refresh=0;
						}
					}
				}
			}
			root.doUpdate();
			//mouse_draw_mousepointerwoxy();
}

extern "C"
void w_dispatch_keyboardevents(char *s) {
	int i,k;

	for (i=0;i<Shell.windowscount;i++) {
		for (k=0;k<Shell.windows[i]->controlscount;k++) {
			if (Shell.windows[i]->controls[k]->handle==Shell.handleWithFocus) {
				Shell.windows[i]->controls[k]->onKeyPressed(s);
				Shell.update();
			}
		}	
	}
}

/*extern "C"
void dispatch_mouseevents(int mousex, int mousey, char leftbuttondown) {
	static char wasdrag;
	static int prevx=mousex;
	static int prevy=mousey;
	int i,k,wnd;
	did=0;
	if ((leftbuttondown == 1) && (wasdrag == 0)) {

			for (i=Shell.windowscount-1;i>=0;i--) {
				 wnd=windowz[i][0];
								
				 if ((Shell.windows[wnd]->invisible == 0) && (Shell.windows[wnd]->minimized == 0)) {	// $$$ Taskbar Window is invisible
					if (mouse_isinside(mousex,
									   mousey,
									   Shell.windows[wnd]->x,
									   Shell.windows[wnd]->y,
									   Shell.windows[wnd]->width,
									   Shell.windows[wnd]->height) == 1) {
					Shell.windows[wnd]->onClick(mousex,mousey,prevx,prevy,Shell.windows[wnd]->x,Shell.windows[wnd]->y,1);
					did=1;
					}
				 }
				 
				 if (Shell.windows[wnd]->minimized != 1) {
				 	if (wnd == Shell.windowWithFocus) {
						for (k=0;k<Shell.windows[wnd]->controlscount;k++) {
						if (mouse_isinside(mousex,
									   	mousey,
									   	Shell.windows[wnd]->controls[k]->x+Shell.windows[wnd]->x+
									   	Shell.windows[wnd]->controls[k]->widthoffset,
									   	Shell.windows[wnd]->controls[k]->y+Shell.windows[wnd]->y+
									   	Shell.windows[wnd]->controls[k]->heightoffset,
									   	Shell.windows[wnd]->controls[k]->width +
									   	Shell.windows[wnd]->controls[k]->width2,
									   	Shell.windows[wnd]->controls[k]->height +
									   	Shell.windows[wnd]->controls[k]->height2) == 1) {
							did=1;
							Shell.windows[wnd]->controls[k]->onClick(mousex,mousey,prevx,prevy,Shell.windows[wnd]->x,Shell.windows[wnd]->y,1);
							Shell.handleWithFocus = Shell.windows[wnd]->controls[k]->handle;
							} else {
								Shell.windows[wnd]->controls[k]->onMouseOut(mousex,mousey,prevx,prevy,1);
							}	
						}
					}
				}
			}		
		
			if (did == 1) Shell.update();
			wasdrag=1;
			prevx=mousex;
			prevy=mousey;
	}
	
	if ((leftbuttondown == 0) && (wasdrag==1)) {
		    did=0;
		    wasdrag=0;
			for (i=0;i<Shell.windowscount;i++) {
					if (Shell.windows[i]->minimized != 1) {
						for (k=0;k<Shell.windows[i]->controlscount;k++) {
							Shell.windows[i]->controls[k]->onRelease(mousex,mousey,prevx,prevy,1);					
						}
				
						Shell.windows[i]->onRelease(mousex,mousey,prevx,prevy,1);										
					}
			}
			
			Shell.update();
	}
	
	if (wasdrag==0) {
		// MOUSE MOVE, ON ACTIVEWINDOW LATER
		for (i=0;i<Shell.windowscount;i++) {
				if (Shell.windows[i]->minimized != 1) {
					for (k=0;k<Shell.windows[i]->controlscount;k++) {
						Shell.windows[i]->controls[k]->onMouseMove(mousex, mousey, Shell.windows[i]->x,Shell.windows[i]->y,leftbuttondown);					
					}
					Shell.windows[i]->onRelease(mousex,mousey,prevx,prevy,1);
				}
		}	
	}
	
	if (Shell.windows[Shell.windowWithFocus]->minimized != 1) {
		Shell.windows[Shell.windowWithFocus]->onMouseMove(mousex, mousey, 0, 0, leftbuttondown);
	}
}*/

extern "C"
void SHinitShell(char *message, char *title, int width, int height) {
	window *w = new window();
	button *b = new button();
	button *b2 = new button();
	textbox *t = new textbox();
	textbox *t2 = new textbox();
	label *l = new label();
	combobox *c = new combobox();
	listbox *li = new listbox();
	menu *me = new menu();
	
	window *w2 = new window();
	taskbar *tb = new taskbar();

	root.init();

	/*w2->createwindow("", SCREENWIDTH, SCREENHEIGHT);
	w2->invisible=1;
	w2->controlscount=0;
	w2->handle=0;
	w2->z=0;*/

	w->createwindow(title, width, height);
	w->invisible=0;
	w->controlscount=0;
	w->handle=0;
	w->z=0;	
		
	b->createbutton("OK", width/2-50, height-30, 100, 22);
	b->handle = 0;
	b->windowHandle = 0;
	b->refresh = 1;
	w->controls[w->controlscount]=(control*)b;
	w->controlscount++;

	b2->createbutton("YES", width/2-50, height-60, 100, 22);
	b2->handle = 1;
	b2->windowHandle = 0;	
	b2->refresh = 1;
	w->controls[w->controlscount]=(control*)b2;
	w->controlscount++;	

	t->createtextbox("TEST", width/2-100, height-100, 200, 21);
	t->windowHandle = 0;
	t->handle = 2;
	t->refresh = 1;
	w->controls[w->controlscount]=(control*)t;
	w->controlscount++;	
	
	t2->createtextbox("TEST", width/2-175, height-150, 350, 21);
	t2->handle = 3;
	t2->windowHandle = 0;
	t2->refresh = 1;
	w->controls[w->controlscount]=(control*)t2;
	w->controlscount++;
			
	l->createlabel(message, 5, 30);
	l->handle = 4;
	l->windowHandle = 0;
	l->refresh = 1;
	w->controls[w->controlscount]=(control*)l;
	w->controlscount++;
	
	c->createcombobox("TEST", width/2-100, height-200, 200, 21);
	c->handle = 5;
	c->windowHandle = 0;
	c->refresh = 1;
	c->additem("TEST1");
	c->additem("TEST2");
	c->additem("TEST3");
	w->controls[w->controlscount]=(control*)c;
	w->controlscount++;	

	li->createlistbox("TEST", width/2-100, height-350, 200, 100);
	li->handle = 6;
	li->windowHandle = 0;
	li->refresh = 1;
	w->controls[w->controlscount]=(control*)li;
	w->controlscount++;
	
	me->createmenu("TEST", 5, 22, width, 22);
	me->handle=7;
	me->windowHandle = 0;
	me->refresh=1;
	me->addtopmenu();
	me->additem("File",0,0);
	me->additem("Exit",0,1);
	me->addtopmenu();
	me->additem("Help",1,0);
	me->additem("About",1,1);
	w->controls[w->controlscount]=(control*)me;
	w->controlscount++;
	
/*	tb->createtaskbar("TASKBAR", 0, SCREENHEIGHT-30, SCREENWIDTH, 30);
	tb->handle=0;
	tb->windowHandle=0;
	tb->refresh=1;
	w2->controls[w2->controlscount]=(control*)tb;
	w2->controlscount++;*/

	Shell.windowscount=0;
	Shell.windows[Shell.windowscount] = w;
	Shell.windowscount++;
/*	Shell.windows[Shell.windowscount] = w2;
	Shell.windowscount++;*/
}

extern "C"
void SHLaunchShell() {
	int k;
	//20180614
	//root.drawWallpaper();
	for (k=0;k<Shell.windowscount;k++) {
		Shell.windows[k]->drawwindow();
	}
	
	Shell.updateAll();	
}


