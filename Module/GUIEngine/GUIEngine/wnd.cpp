/*
	LikeOS Windowing System
	Copyright(c) 2005 Nikolaos Rangos
*/

//#include "atapi.h"
//#include "fat32.h"
#include "wnd.h"
#include "windef.h"
#include "txtCtrl.h"
#include "font.h"
#include  "fileio.h"
#include "VESA.h"

unsigned char *wndtop;
unsigned char *wndbtm;
unsigned char *wndleft;
unsigned char *wndright;
unsigned char *wndlt;
unsigned char *wndrt;
unsigned char *wndrb;
unsigned char *wndlb;
unsigned char *wndctl;
unsigned char *iterm; // terminal icon
unsigned char *wallpaper1;
unsigned short window_handle_counter=0;

unsigned long *offscreen1;	// double buffer
unsigned long *backrect;	// rect back buffer
int num_windows=0;

extern struct WND * wnd_head;

unsigned long *getOffscreenBuffer() {
	return (unsigned long*)offscreen1;
}

void gui_init(unsigned char *wallpaper_ptr) {
	/*	wallpaper1 = wallpaper_ptr;
	
	wndtop=(unsigned char*) kmalloc(cdiso_getfilesize("WNDTOP.RAW"));
	cdiso_readfile("WNDTOP.RAW", wndtop);

	wndbtm=(unsigned char*) kmalloc(cdiso_getfilesize("WNDBTM.RAW"));
	cdiso_readfile("WNDBTM.RAW", wndbtm);
	
	wndleft=(unsigned char*) kmalloc(cdiso_getfilesize("WNDLEFT.RAW"));
	cdiso_readfile("WNDLEFT.RAW", wndleft);
	
	wndright=(unsigned char*) kmalloc(cdiso_getfilesize("WNDRIGHT.RAW"));
	cdiso_readfile("WNDRIGHT.RAW", wndright);
	
	wndlt=(unsigned char*) kmalloc(cdiso_getfilesize("WNDLT.RAW"));
	cdiso_readfile("WNDLT.RAW", wndlt);
		
	wndrt=(unsigned char*) kmalloc(cdiso_getfilesize("WNDRT.RAW"));
	cdiso_readfile("WNDRT.RAW", wndrt);
		
	wndlb=(unsigned char*) kmalloc(cdiso_getfilesize("WNDLB.RAW"));
	cdiso_readfile("WNDLB.RAW", wndlb);
		
	wndrb=(unsigned char*) kmalloc(cdiso_getfilesize("WNDRB.RAW"));
	cdiso_readfile("WNDRB.RAW", wndrb);
	
	wndctl=(unsigned char*) kmalloc(cdiso_getfilesize("WNDCTL.RAW"));
	cdiso_readfile("WNDCTL.RAW", wndctl);
	
	iterm=(unsigned char*) kmalloc(cdiso_getfilesize("ITERM.RAW"));
	cdiso_readfile("ITERM.RAW", iterm);
	*/

	/*wallpaper1 = wallpaper_ptr;

	FILE* fp = fopen("WNDTOP.RAW", "rb");
	wndtop=(unsigned char*)kmalloc(fp->_fileLength);
	fread(wndtop, fp->_fileLength, 1, fp);
	fclose(fp);

	fp = fopen("WNDBTM.RAW", "rb");
	wndbtm=(unsigned char*) kmalloc(fp->_fileLength);
	fread(wndbtm, fp->_fileLength, 1, fp);
	fclose(fp);

	fp = fopen("WNDLEFT.RAW", "rb");
	wndleft=(unsigned char*)kmalloc(fp->_fileLength);
	fread(wndleft, fp->_fileLength, 1, fp);
	fclose(fp);

	fp = fopen("WNDRIGHT.RAW", "rb");
	wndright=(unsigned char*)kmalloc(fp->_fileLength);
	fread(wndright, fp->_fileLength, 1, fp);
	fclose(fp);

	fp = fopen("WNDLT.RAW", "rb");
	wndlt=(unsigned char*)kmalloc(fp->_fileLength);
	fread(wndlt, fp->_fileLength, 1, fp);
	fclose(fp);

	fp = fopen("WNDRT.RAW", "rb");
	wndrt=(unsigned char*)kmalloc(fp->_fileLength);
	fread(wndrt, fp->_fileLength, 1, fp);
	fclose(fp);

	fp = fopen("WNDLB.RAW", "rb");
	wndlb=(unsigned char*)kmalloc(fp->_fileLength);
	fread(wndlb, fp->_fileLength, 1, fp);
	fclose(fp);

	fp = fopen("WNDRB.RAW", "rb");
	wndrb=(unsigned char*)kmalloc(fp->_fileLength);
	fread(wndrb, fp->_fileLength, 1, fp);
	fclose(fp);

	fp = fopen("WNDCTL.RAW", "rb");
	wndctl=(unsigned char*)kmalloc(fp->_fileLength);
	fread(wndctl, fp->_fileLength, 1, fp);
	fclose(fp);

	fp = fopen("ITERM.RAW", "rb");
	iterm=(unsigned char*)kmalloc(fp->_fileLength);
	fread(iterm, fp->_fileLength, 1, fp);
	fclose(fp);
	
	*/
	// root window, invisible 
	wnd_head= new WND;
	wnd_head->prev=NULL;
	wnd_head->next=NULL;
	wnd_head->rect.x = 0;
	wnd_head->rect.y = 0;
	wnd_head->rect.width = 0;
	wnd_head->rect.height = 0;
	wnd_head->caption = 0;
	wnd_head->needs_repaint = FALSE;
	wnd_head->prev_ctrl=NULL;
	wnd_head->next_ctrl=NULL;
	wnd_head->first_ctrl=NULL;
	wnd_head->control_struct=NULL;
	
	offscreen1=(unsigned long*) new char[1024*768*4];
	//backrect=(unsigned long*) kmalloc(10);
	//backrect=(unsigned long*)kmalloc(((search->rect.width*2)+(search->rect.height*2))*3);
	backrect=(unsigned long*)new char[1024*768*4];
	//init_bar();	
}

void DrawToken(unsigned char *buffer, unsigned long *target, int x2, int y2, int width, int height) {
	unsigned char r,g,b;
	int i,x,y;
	i=0;

	for (y=0; y<height; y++) {
		if ((y+y2>768) || (y+y2<0)) continue;
		i=width*y*3;
		for (x=0; x<width; x++) {
			if ((x+x2>1024) || (x+x2<0)) continue;
			r=buffer[i];
			g=buffer[i+1];
			b=buffer[i+2];
			i+=3;
						
			target[1024*(y+y2)+(x+x2)] = (r<<16) | (g<<8) | b;
		}
	}	
}

void DrawWindow(int x, int y, int width, int height) {
	int i,x2,y2;
	unsigned char r,g,b;
	char first=0;
	unsigned long col=(31<<16) | (42<<8) | 49;
	
	for (y2=0; y2<height; y2++) {
		if ((y+y2>768) || (y+y2<0)) continue;
		for (x2=0; x2<width; x2++) {
			if ((x+x2>1024) || (x+x2<0)) continue;
			offscreen1[1024*(y2+y)+(x2+x)] = col;
		}
	}	
	
	for (i=0; i<(width+WNDRIGHTX)/WNDTOPX; i++) {
		DrawToken(wndtop, offscreen1, x+i*WNDTOPX, y, WNDTOPX, WNDTOPY);
	}
	
	for (i=0; i<(width+WNDRIGHTX)/WNDBTMX; i++) {
		DrawToken(wndbtm, offscreen1, x+i*WNDBTMX, y+height-WNDBTMY, WNDBTMX, WNDBTMY);
	}
	
	for (i=0; i<height/WNDLEFTY; i++) {
		DrawToken(wndleft, offscreen1, x, y+i*WNDLEFTY, WNDLEFTX, WNDLEFTY);
	}
	
	for (i=0; i<height/WNDRIGHTY; i++) {
		DrawToken(wndright, offscreen1, x+width, y+i*WNDRIGHTY, WNDRIGHTX, WNDRIGHTY);
	}
	
	DrawToken(wndlt, offscreen1, x, y, WNDLTX, WNDLTY);
	DrawToken(wndrt, offscreen1, x+width-WNDRTX+WNDRIGHTX, y, WNDRTX, WNDRTY);

	DrawToken(wndlb, offscreen1, x, y+height-WNDLBY, WNDLBX, WNDLBY);
	DrawToken(wndrb, offscreen1, x+width-WNDRBX+WNDRIGHTX, y+height-WNDRBY, WNDRBX, WNDRBY);
	DrawToken(wndctl, offscreen1, x+width-WNDCTLX, y+3, WNDCTLX, WNDCTLY);
	DrawToken(iterm, offscreen1, x+3, y+3, ITERMX, ITERMY);
}

struct WND *wnd_append(struct WND *wnd_append) {
	struct WND *search;
	search=wnd_head;
	wnd_append->next=NULL;
	
	while (search->next != NULL) {
		search=search->next;
	}

	search->next=wnd_append;
	wnd_append->prev=search;

	return wnd_append;
}

void wnd_update() {
	unsigned char r,g,b;
	unsigned long i,x;
	i=0;
	
	for (x=0; x<1024*768; x++) {

		if (wallpaper1 != nullptr)
		{
			r = wallpaper1[i];
			g = wallpaper1[i + 1];
			b = wallpaper1[i + 2];
			i += 3;

			offscreen1[x] = (r << 16) | (g << 8) | b;
		}
			
		
	}	
	
	struct WND *search;
	struct WND *ctrl;
	
	search=wnd_head;
	do {
		search=search->next;
		DrawWindow(search->rect.x, search->rect.y, search->rect.width, search->rect.height);
		
		putstring((char*)search->caption, search->rect.x+25, search->rect.y+3, offscreen1);
		
		if (search->first_ctrl != NULL) {
			ctrl=search->first_ctrl;
			while(1) {
				void (*control_update_function)(struct WND *ctrl,
											    struct WND *window,
											    unsigned char uselfb,
											    unsigned fromKbdDriver);

				control_update_function = (void(__cdecl *)(WND *, WND *, unsigned char, unsigned int))ctrl->control_update_function;
				control_update_function(ctrl, search, 0, 0);
				
				if (ctrl->next_ctrl == NULL) break;
				ctrl=ctrl->next_ctrl;	
			}
		}
		
	} while (search->next != NULL);
	
	lfb_update(offscreen1);
}

void WNDSystem_SetTopWindow(int handle) {
	struct WND *search, *search2;
	struct WND *this_window;
	
	search=wnd_head;
	
	// find window, delete in chain and append at the end of chain
	do {
		search=search->next;		
		if (search->handle == handle) {
			this_window = search;
			search2 = search->prev;

			if (search2 != nullptr)
			{
				search2->next = search->next;
				search2 = search->next;

				if(search2 != nullptr)
					search2->prev = search->prev;
				
			}
			break;
			
		}
	} while (search->next != NULL);	

	wnd_append(this_window);

	search=wnd_head;
	do {
		search=search->next;
		search->is_topmost_window=0;
		search->active_window=0;		
	} while (search->next != NULL);	
		
	this_window->is_topmost_window=1;
	this_window->active_window=1;
}

struct WND *NewWindow(char *caption, int x, int y, int width, int height) {
	struct WND *newwindow= new WND;	
	
	newwindow->rect.x = x;
	newwindow->rect.y = y;
	newwindow->rect.width = width;
	newwindow->rect.height = height;
	newwindow->caption = (unsigned char*)caption;
	newwindow->needs_repaint = FALSE;
	newwindow->handle = ++window_handle_counter;
	newwindow->prev_ctrl=NULL;
	newwindow->next_ctrl=NULL;
	newwindow->first_ctrl=NULL;
	
	num_windows++;
		
	wnd_append(newwindow);
	WNDSystem_SetTopWindow(newwindow->handle);

	return newwindow;
}

int WNDSystem_Windowtop_Clicked(struct WND *wnd, int x, int y) {
	if ((x >= wnd->rect.x) && (x <= wnd->rect.x + wnd->rect.width) &&
	    (y <= (wnd->rect.y+WNDTOPY)) && (y >= wnd->rect.y)) {
		return TRUE;
	}
	
	return FALSE;
}

int WNDSystem_Window_Clicked(struct WND *wnd, int x, int y) {
	if ((x >= wnd->rect.x) && (x <= wnd->rect.x + wnd->rect.width) &&
	    (y <= (wnd->rect.y+wnd->rect.height)) && (y >= wnd->rect.y)) {
		return TRUE;
	}	

	return FALSE;	
}

int WNDSystem_Windowresize_Clicked(struct WND *wnd, int x, int y) {
	
	if ( (x >= wnd->rect.x + (wnd->rect.width-15))
	   &&(x <= wnd->rect.x + wnd->rect.width)
	   &&(y <= wnd->rect.y + (wnd->rect.height))
	   &&(y >= wnd->rect.y + (wnd->rect.height-15)) ) {
			return TRUE;  
	   }
	
	return FALSE;	
}

void SaveRect(int x, int y, int width, int height) {
	int x2,y2;

	for (x2=0; x2<width; x2++)
		backrect[x2] = getp(1024*y+(x+x2));
		
	for (x2=0; x2<width; x2++)
		backrect[width+x2] = getp(1024*(y+height)+(x+x2));
		
	for (y2=0; y2<height; y2++)
		backrect[width*2+y2] = getp(1024*(y+y2)+x);
		
	for (y2=0; y2<height; y2++)
		backrect[width*3+y2] = getp(1024*(y+y2)+(x+width));	
}

void DrawBackRect(int x, int y, int width, int height) {
	int x2,y2;

	for (x2=0; x2<width; x2++)
		putp(1024*y+(x+x2), backrect[x2]);
		
	for (x2=0; x2<width; x2++)
		putp(1024*(y+height)+(x+x2), backrect[width+x2]);
		
	for (y2=0; y2<height; y2++)
		putp(1024*(y+y2)+x, backrect[width*2+y2]);
		
	for (y2=0; y2<height; y2++)
		putp(1024*(y+y2)+(x+width), backrect[width*3+y2]);
}

void DrawRect(int x, int y, int width, int height, int oldx, int oldy, int oldwidth, int oldheight) {
	int x2, y2;

	DrawBackRect(oldx, oldy, oldwidth, oldheight);
	SaveRect(x, y, width, height);
	//************************************************
	// draw rect
	//************************************************
	if (!(y>1024 || y < 0)) {
		for (x2=0; x2<width; x2+=2) {
			if (x+x2>1024 || x+x2 < 0) continue;
			putp(1024*y+(x+x2), 0);
		}
	}

	if (!(y+height>768 || y+height < 0)) {
		for (x2=0; x2<width; x2+=2) {
			if (x+x2>1024 || x+x2 < 0) continue;
			putp(1024*(y+height)+(x+x2), 0);
		}
	}

	if (!(x>1024 || x < 0)) {
		for (y2=0; y2<height; y2+=2) {
			if (y+y2>768 || y+y2 < 0) continue;
			putp(1024*(y+y2)+x, 0);
		}
	}
	
	if (!(x+width>1024 || x+width < 0)) {
		for (y2=0; y2<height; y2+=2) {
			if (y+y2>768 || y+y2 < 0) continue;			
			putp(1024*(y+y2)+(x+width), 0);
		}
	}
}

int WNDSystem_ChildControl_Clicked(struct WND *ctrl, int x, int y) {
	int i=0;
	while (1) {
		if (ctrl->clickhandlers[i].handler==NULL) break;
		
		if ((x >= ctrl->clickhandlers[i].x) && 
			(x <= ctrl->clickhandlers[i].x + ctrl->clickhandlers[i].width) &&
	    	(y <= (ctrl->clickhandlers[i].y+ctrl->clickhandlers[i].height)) && 
	    	(y >= ctrl->clickhandlers[i].y)) {
			return i;
		}	
		
		i++;
	}
	return -1;	
}

char isfirst=0;

void WNDSystem_Left_MouseUp(int x, int y, int x2, int y2) {
	struct WND *search;
	struct WND *ctrl;
	char found=0;
	search=wnd_head;
	int tmp1,tmp2,oldw,oldh,ret;
	
	do {
		search=search->next;
		
		if (WNDSystem_Windowresize_Clicked(search, x, y) == TRUE) {
				found=2;
				break;
		}		
		
		if (WNDSystem_Windowtop_Clicked(search, x, y) == TRUE) {
			found=1;
			break;
		}
		
		if (search->first_ctrl != NULL) {
			ctrl=search->first_ctrl;
			while(1) {
				if ((ret=WNDSystem_ChildControl_Clicked(ctrl, x2, y2)) != -1) {
				void (*clickhandler)(struct WND *ctrl);
					clickhandler = (void(__cdecl *)(WND *))(ctrl->clickhandlers[ret].handler);
					clickhandler(ctrl);					
				}				
				if (ctrl->next_ctrl == NULL) break;				
				ctrl=ctrl->next_ctrl;
			}
		}				
	} while (search->next != NULL);
	
	if (found==1) {		// window top clicked
		search->rect.x = x2-(search->rect.width/2);
		search->rect.y = y2;
	}
	
	if (found==2) {		// window resize clicked
		
		oldw=search->rect.width;
		oldh=search->rect.height;
		search->rect.width += x2-x;
		search->rect.height += y2-y;
		
		if (search->first_ctrl != NULL) {
			ctrl=search->first_ctrl;
			while(1) {
				tmp1=ctrl->rect.width + search->rect.width - oldw;
				tmp2=ctrl->rect.height + search->rect.height - oldh;
					ctrl->rect.width = tmp1;
					ctrl->rect.height = tmp2;
				if (ctrl->next_ctrl == NULL) break;				
				ctrl=ctrl->next_ctrl;
			}
		}		
	}

	wnd_update();
	isfirst=0;
}

char clicked_topmost=0;
unsigned int saved_handle=0;

void WNDSystem_Left_MouseDown(int x, int y, int x2, int y2, int x3, int y3, char drag) {
	struct WND *search;
	struct WND *ctrl;
	char found=0;
	search=wnd_head;
	
	if (drag == 1) {
		do {
			search=search->next;		
			if (WNDSystem_Windowresize_Clicked(search, x, y) == TRUE) {
				found=2;
				break;
			}			
			
			if (WNDSystem_Windowtop_Clicked(search, x, y) == TRUE) {
				found=1;
				break;
			}
		} while (search->next != NULL); // wird das letzte im chain evt. nicht behandelt?
	
		if (found==1) {	// top clicked
			if (isfirst == 0) {
				SaveRect(x3-(search->rect.width/2), y3-10, search->rect.width, search->rect.height);
				isfirst=1;	
			}
			
			DrawRect(x2-(search->rect.width/2),y2-10,
					 search->rect.width,search->rect.height,
					 x3-(search->rect.width/2), y3-10,
					 search->rect.width,search->rect.height
					 );
		}
		
		if (found == 2) {	// resize
			if (isfirst == 0) {
				SaveRect(x3-(search->rect.width/2), y3-10, search->rect.width, search->rect.height);
				isfirst=1;	
			}
			
			DrawRect(search->rect.x,search->rect.y,
					 search->rect.width+x2-x,search->rect.height+y2-y,
					 search->rect.x,search->rect.y, search->rect.width+x3-x, search->rect.height+y3-y
   		    		);
		}
		
	} else {
		clicked_topmost=0;
		saved_handle=0;
		do {
			search=search->next;		
			if (WNDSystem_Window_Clicked(search, x2, y2) == TRUE) {
				found=1;
				saved_handle=search->handle;
				if (search->is_topmost_window == 1) {
					clicked_topmost=1;
				}
			}
		} while (search->next != NULL);
		
		if ((found == 1) && (clicked_topmost == 0)) {
			WNDSystem_SetTopWindow(saved_handle);
			wnd_update();
		}
	}	
}
