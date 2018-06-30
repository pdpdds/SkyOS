#include <wnd.h>
#include "windef.h"
#include "txtCtrl.h"
#include "font.h"
#include "string.h"
#include "VESA.h"
//#include "atapi.h"
//#include "fat32.h"

#define MAGICY window->rect.y+y+WNDTOPY+1
#define MAGICX window->rect.x+x+WNDLEFTX+1
#define SCROLLHEIGHT 14
#define SCROLLWIDTH 14

unsigned char *scrolltop;
unsigned char *scrollbottom;

void DrawToken_scrollbars(unsigned char *buffer, 
						  unsigned char *target,
						  int x2,
						  int y2,
						  int width,
						  int height,
						  int rectwidth,
						  int rectheight) {
	unsigned char r,g,b;
	int i,x,y;
	i=0;

	for (y=0; y<height; y++) {
		if ((y+y2>768) || (y+y2<0)) continue;
		i=width*y;
		for (x=0; x<width; x++) {
			if ((x+x2>1024) || (x+x2<0)) continue;
			target[rectwidth*(y+y2)*3+(x+x2)*3] = buffer[width*y*3+x*3];
			target[rectwidth*(y+y2)*3+(x+x2)*3+1] = buffer[width*y*3+x*3+1];
			target[rectwidth*(y+y2)*3+(x+x2)*3+2] = buffer[width*y*3+x*3+2];
		}
	}	
}

unsigned char *truncate_for_breaks(struct textcontrol_t *txtctrl, unsigned char *buffer, unsigned int maxbreaks) {
	int i,k,j,m,n,found=0;
	
	for (i=0,k=0; i<strlen((const char*)buffer); i++) {
		if (buffer[i]=='\r') k++;	
	}
	
	if (k<=maxbreaks) return buffer;

	maxbreaks+=txtctrl->scroll;
	if (maxbreaks > k) {
		maxbreaks=k;
		txtctrl->scroll--;
	}

	for (i=0,j=0; i<strlen((const char*)buffer); i++) {
		if (buffer[i]=='\r') j++;
		if (j >= (k-maxbreaks)) {
			found=1;
			m=i;
			break;
		}
	}

	n=0;
	if (txtctrl->scroll != 0) {
		for (i=0,j=0; i<strlen((const char*)buffer); i++) {
			if (buffer[i]=='\r') j++;
			if (j >= (k-txtctrl->scroll)) {
				found=1;
				n=i;
				break;
			}
		}	
	}
		
	if (found==1) {
		if (n!=0) buffer[n]=0;
		return &buffer[m+1];
	}

	return nullptr;
}

void handler_scrollbar_up(struct WND *ctrl) {
	struct textcontrol_t *txtctrl=(struct textcontrol_t*) ctrl->control_struct;
	txtctrl->scroll++;
}

void handler_scrollbar_down(struct WND *ctrl) {
	struct textcontrol_t *txtctrl=(struct textcontrol_t*) ctrl->control_struct;
	if (txtctrl->scroll != 0)
		txtctrl->scroll--;
}

void textCtrl_install_clickhandlers(struct WND *window, struct WND *ctrl) {
	ctrl->clickhandlers[0].handler=handler_scrollbar_up;
	ctrl->clickhandlers[0].x = window->rect.x - WNDRIGHTX + ctrl->rect.x + ctrl->rect.width-SCROLLWIDTH;
	ctrl->clickhandlers[0].y = window->rect.y + WNDTOPY;
	ctrl->clickhandlers[0].width = SCROLLWIDTH;
	ctrl->clickhandlers[0].height = SCROLLHEIGHT;
	
	ctrl->clickhandlers[1].handler=handler_scrollbar_down;
	ctrl->clickhandlers[1].x = window->rect.x - WNDRIGHTX + ctrl->rect.x + ctrl->rect.width-SCROLLWIDTH;
	ctrl->clickhandlers[1].y = window->rect.y + WNDTOPY + ctrl->rect.y + ctrl->rect.height - SCROLLHEIGHT;
	ctrl->clickhandlers[1].width = SCROLLWIDTH;
	ctrl->clickhandlers[1].height = SCROLLHEIGHT;	
}

unsigned char repaint_me=0;
void txtCtrl_Update(struct WND *ctrl, struct WND *window, unsigned char uselfb, unsigned fromKbdDriver) {
	int x,y,i=0;
	unsigned char r,g,b;
	unsigned long where;
	unsigned long *offscreen = (uselfb == 1) ? getlfb() : getOffscreenBuffer();
	unsigned fin=0;
	char *p,*p2;
	char *buf=(char*) new char[65535];
	unsigned char *bitmap;
	struct textcontrol_t *txtctrl=(struct textcontrol_t*) ctrl->control_struct;
	int width, height;
	unsigned int breaks;
	
	width=ctrl->rect.width;
	height=ctrl->rect.height;	
	textCtrl_install_clickhandlers(window, ctrl);
	
	if (fromKbdDriver == 0) {
		bitmap=(unsigned char*) new char[width*height*3];
	
		if (width>=window->rect.width)
			ctrl->rect.width=window->rect.width-WNDRIGHTX;

		if (height>=window->rect.height)
			ctrl->rect.height=window->rect.height-WNDBTMY-WNDTOPY;	
	
		for (i=0; i<width*height*3; i++) {
			bitmap[i]=0xf0;
		}

		DrawToken_scrollbars(scrolltop, bitmap, width-SCROLLWIDTH, 0, SCROLLWIDTH, SCROLLHEIGHT, width, height);
		DrawToken_scrollbars(scrollbottom, bitmap, width-SCROLLWIDTH, height-SCROLLHEIGHT, SCROLLWIDTH, SCROLLHEIGHT,
							 width, height);
		
		for (y=0; y<ctrl->rect.height; y++) {
			if (MAGICY > 1024 || MAGICY<0) continue;
			i=ctrl->rect.width*y*3;
			for (x=0; x<ctrl->rect.width; x++) {
			if (MAGICX > 1024 || MAGICX<0) continue;
				r=bitmap[i];
				g=bitmap[i+1];
				b=bitmap[i+2];
				i+=3;

				where=1024*(MAGICY)+(MAGICX);
				offscreen[where] = (r<<16) | (g<<8) | b;
			}
		}
	}
	
	strcpy(buf, (const char*)txtctrl->keyboard_buffer);
	p2=(char*)truncate_for_breaks(txtctrl, (unsigned char*)buf, (height-FONT_HEIGHT)/FONT_HEIGHT);
	//p2=buf;
	y=window->rect.y+WNDTOPY+1;
	
	while (1) {	
		for (p=p2;;p++) {
				if (*p==0x0d) break;
				if (*p==0) {
					fin=1;
					break;	
				}
		}
		*p=0;
		if ((fin==0) && (*(p+1)==0) && (fromKbdDriver == 1)){
			void (*command_callback)(struct textcontrol_t *txtctrl,
									 char *command);
			
			command_callback = (void(__cdecl *)(textcontrol_t *, char *))txtctrl->command_callback;
			command_callback(txtctrl, p2);
			repaint_me=1;
			break;
		}
		
		putstring(p2, window->rect.x+WNDLEFTX+1, y, offscreen);
		if (fin==1) break;
		p++;
		p2=p;
		y+=FONT_HEIGHT;
	}
	
	delete buf;
	if (fromKbdDriver == 0) delete bitmap;
	
	if (repaint_me==1) {
		// repaint for command output
		repaint_me=0;
		txtCtrl_Update(ctrl, window, uselfb, 0);
	}
	
}

void addTextControl(struct WND *window, int x, int y, int width, int height, void *callback) {
	struct WND *ctrl = new WND;
	struct textcontrol_t *textcontrol_struct= new textcontrol_t;
	unsigned long i=0;

	if (width>=window->rect.width)
		width=window->rect.width-WNDRIGHTX;

	if (height>=window->rect.height)
		height=window->rect.height-WNDBTMY-WNDTOPY;		
	
	textcontrol_struct->keyboard_buffer=(unsigned char*) new char[65535];
	textcontrol_struct->command_callback=callback;	
			
	struct WND *search=window->first_ctrl;
	
	if (window->first_ctrl == NULL) {
		search=window;	
		search->first_ctrl=ctrl;
		goto is_first_control;
	}
	
	while (search->next_ctrl != NULL) {
		search=search->next_ctrl;
	} 
	
	search->next_ctrl=ctrl;
	ctrl->prev_ctrl=search;

is_first_control:
	
	ctrl->next=NULL;
	ctrl->rect.x=x;
	ctrl->rect.y=y;
	ctrl->rect.width=width;
	ctrl->rect.height=height;
	ctrl->control_struct = (struct textcontrol_t *)textcontrol_struct;
	ctrl->control_type = CONTROL_TYPE_TEXTCONTROL;
	ctrl->control_update_function=txtCtrl_Update;
	ctrl->needs_keyboard=1;
	textcontrol_struct->scroll=0;
	textCtrl_install_clickhandlers(window, ctrl);
}

void init_textCtrl() {
	/*scrolltop=(unsigned char*)kmalloc(cdiso_getfilesize("SCLTOP.RAW"));
	cdiso_readfile("SCLTOP.RAW", scrolltop);
		
	scrollbottom=(unsigned char*)kmalloc(cdiso_getfilesize("SCLBTM.RAW"));
	cdiso_readfile("SCLBTM.RAW", scrollbottom);*/
}
