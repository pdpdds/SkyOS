/*
**	PS/2 Mouse Driver
**	Copyright (C) 2005-2006 Nikolaos Rangos
*/

#include "windef.h"
#include "Hal.h"
#include "VESA.h"

#define MOUSE_COMMAND 0x60
#define MOUSE_COMMAND2 0x64
#define MOUSE_DATA 0x64
#define MOUSE_COMMAND_STREAMMODE 0xea
#define MOUSE_COMMAND_ACTIVATE 0xf4
#define MOUSE_COMMAND_SETSTANDART 0xf6
#define MOUSE_COMMAND_ZUSATZEINHEIT 0xd4
#define MOUSE_COMMAND_ENABLE_INTERRUPT 0x01
#define MOUSE_SIZEX 12
#define MOUSE_SIZEY 21

#undef VIRTUALMACHINE
#define VIRTUALMACHINE

#define SCREENWIDTH 1024
#define SCREENHEIGHT 768

int mouse_x=0;
int mouse_y=0;
int width=SCREENWIDTH;
int height=SCREENHEIGHT;

unsigned long *mouseback;
unsigned char *mousebuffer;

char left_button_state=0;
int oldx,oldy,curx,cury;

void mouse_draw_mousepointer(int x2, int y2) {
	unsigned char r,g,b;
	int i,x,y;
	i=0;
	
	for (y=0; y<MOUSE_SIZEY; y++) {
		i=MOUSE_SIZEX*y*3;
		for (x=0; x<MOUSE_SIZEX; x++) {
			r=mousebuffer[i];
			g=mousebuffer[i+1];
			b=mousebuffer[i+2];
			i+=3;
			
			mouseback[MOUSE_SIZEX*y+x] = getp(SCREENWIDTH*(y+y2)+(x+x2));
			if ((r != 121) && (g != 27) && (b != 27))	{		
				pp(SCREENWIDTH*(y+y2)+(x+x2), r, g, b);
			}
		}
	}
}

extern "C" void mouse_draw_mousepointerwoxy() {
	mouse_draw_mousepointer(curx, cury);
}

void mouse_draw_mouseback(int x2, int y2) {	
	int i,x,y;
	i=0;
	
	for (y=0; y<MOUSE_SIZEY; y++) {
		i=MOUSE_SIZEX*y*3;
		for (x=0; x<MOUSE_SIZEX; x++) {
			putp(SCREENWIDTH*(y+y2)+(x+x2), mouseback[MOUSE_SIZEX*y+x]);
		}
	}
}

int mouse_x2, mouse_y2;

void mouse_update_cursor(short x_movement,
						 short y_movement,
						 char x_negative,
						 char y_negative) {
				 
	/*if (x_negative == 1) {
		x_movement -= 256;
	}

	if (y_negative == 1) {
		y_movement -= 256;
	}*/
	
	if (((mouse_x + x_movement) < (width-MOUSE_SIZEX))	&& ((mouse_y - y_movement) < height)
	   && ((mouse_x + x_movement) > 0) && ((mouse_y - y_movement) > 0)) {		    
			mouse_x += x_movement;
			mouse_y -= y_movement;
						
			mouse_draw_mouseback(mouse_x2, mouse_y2);
			mouse_draw_mousepointer(mouse_x, mouse_y);	
	}
}

unsigned char mouse_cycle=0;    //unsigned char
char mouse_byte[4];    			//signed char
void mouse_handler(char mouse_data, char x_movement, char y_movement);
char mouse_haswheel=0;		// zero indicates mouse has no wheel

char mousemoves=0;
void interrupt_mouse();
void interrupt_mouse2();
extern void SendEOI();
__declspec(naked) void kSVGAMouseHandler()
{

	_asm {
		PUSHAD
		PUSHFD
		CLI
	}

	_asm
	{
		call interrupt_mouse2
	}

	SendEOI();

	_asm
	{

		POPFD
		POPAD
		IRETD
	}
}

char left_button = 0;
char left_button_up = 0;
char right_button = 0;
char middle_button = 0;
char oleftbutton = 0;

#include "SkyOS.h"
#include "SkyGUISystem.h"
extern void SampleFillRect(ULONG* lfb, int x, int y, int w, int h, int col);
int pos = 0;

MOUSE_DEC mdec;
extern int DecodeMouseValue(MOUSE_DEC *mdec, unsigned char dat);
extern void set_mouse_position(int x, int y);
void interrupt_mouse2()
{
	int data;
	OutPortByte(PIC1_OCW2, 0x64);	/* IRQ-12 접수 완료를 PIC1에 통지 */
	OutPortByte(PIC0_OCW2, 0x62);	/* IRQ-02 접수 완료를 PIC0에 통지 */
	data = InPortByte(PORT_KEYDAT);

	char x_negative;
	char y_negative;
	//	unsigned char status;
	//	unsigned char x_movement; // byte two
	//	unsigned char y_movement; // byte three
	//if ((InPortByte(0x64) & 0x21) != 0x21) return;	// check if output buffer full

	

	
	//if ((data & 0x01) == 0x01) left_button = 1;
	//if ((data & 0x02) == 0x02) right_button = 1;
	if ((data & 0x10)) x_negative = 1;
	if ((data & 0x20)) y_negative = 1;
	
	if (DecodeMouseValue(&mdec, data) != 0)
	{
		mousemoves = 1;

		if ((mdec.btn & 0x01) != 0)  //왼쪽 버튼을 눌렀다면 마우스 바로 아래의 윈도우를 드래그 처리한다.
		{
			left_button = 1;
		}
		
		if ((mdec.btn & 0x02) != 0) //오른쪽 버튼을 눌렀다면 콘솔 프로세스를 생성한다.
			right_button = 1;



		
		mouse_x += mdec.x;
		mouse_y += mdec.y;

		mouse_x2 = curx = mouse_x;
		mouse_y2 = cury = mouse_y;

		

		
		if (mouse_x < 0) {
			mouse_x = 0;
		}
		if (mouse_y < 0) {
			mouse_y = 0;
		}
		if (mouse_x > 1024 - 1) {
			mouse_x = 1024 - 1;
		}
		if (mouse_y > 768 - 1) {
			mouse_y = 768 - 1;
		}
		
		/*int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF };
		ULONG* lfAb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;


		SampleFillRect(lfAb, mouse_x, mouse_y, 20, 20, colorStatus[pos]);
		if (++pos > 2)
			pos = 0;*/
		set_mouse_position(mouse_x, mouse_y);
		//mouse_update_cursor(mdec.x, mdec.y, x_negative, y_negative);
	}
	else
	{
		if(left_button == 1)
			left_button_up = 1;

		left_button = 0;
		right_button = 0;

		x_negative = 0;
		y_negative = 0;
	}
}


void interrupt_mouse()
{

	
  static unsigned char mousecount=0;  
  mousemoves=0;
	if ((InPortByte(0x64) & 0x01) != 0) {
    	mouse_byte[mousecount++]= InPortByte(0x60);
    	if (mousecount >= 3+mouse_haswheel) {
	    	mousecount=0;
	    	mousemoves=1;
#ifndef VIRTUALMACHINE
	    		mouse_handler(mouse_byte[0], mouse_byte[1], mouse_byte[2]);
#else
	    		mouse_handler(mouse_byte[2], mouse_byte[0], mouse_byte[1]);
#endif
    	}
	}
}

extern "C" int mouse_getx() {
	return mouse_x2;	
}

extern "C" int mouse_gety() {
	return mouse_y2;
}



extern "C" int mouse_getbutton() {
	
	if (left_button == 0) {
		return 0;
	}	
	
	if (left_button==1) {
		return 1;
	}
		
	if (right_button == 1) {
		right_button = 0;
		return 2;	
	}
	
	return 3;
}

extern "C" void mouse_setposition(int x, int y) {
	mouse_x2=curx=x;
	mouse_y2=cury=y;
}

extern "C" int mouse_update() {

	if (left_button_up == 1)
	{
		left_button_up = 0;
		return 1;
	}


	return mousemoves;
}

char wasdrag=0;
void mouse_handler(char mouse_data, char x_movement, char y_movement) {
//	char mouse_data;
	char x_negative;
	char y_negative;
//	unsigned char status;
//	unsigned char x_movement; // byte two
//	unsigned char y_movement; // byte three
	//if ((InPortByte(0x64) & 0x21) != 0x21) return;	// check if output buffer full
	
	x_negative=0;
	y_negative=0;
	
	left_button = 0;
	right_button = 0;
	if ((mouse_data & 0x01) == 0x01) left_button=1;
	if ((mouse_data & 0x02) == 0x02) right_button=1;
	if ((mouse_data & 0x10)) x_negative=1;
	if ((mouse_data & 0x20)) y_negative=1;

	/*int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF };
	ULONG* lfAb = (ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;


	SampleFillRect(lfAb, mouse_x, mouse_y, 20, 20, colorStatus[pos]);
	if (++pos > 2)
		pos = 0;*/

	if (((mouse_x + x_movement) < (width-MOUSE_SIZEX))	&& ((mouse_y - y_movement) < height)
	   && ((mouse_x + x_movement) > 0) && ((mouse_y - y_movement) > 0)) {		

		
		
		mouse_x += x_movement;
		mouse_y -= y_movement;

		mouse_x2=curx=mouse_x;
		mouse_y2=cury=mouse_y;

	}		
	//dispatch_mouseevents(mouse_x2, mouse_y2, left_button);
	//mouse_update_cursor(x_movement, y_movement, x_negative, y_negative);	
}
/*
void mouse_waitdata() {
	while ((InPortByte(MOUSE_DATA) & 0x02) == 0x02);
}
*/

/*void mouse_mousepointer_init() {	
	int fd = file_open("/MOUSE.RAW");
	if (fd != 1) {
		panic1("Could not read from device");	
	}
	mousebuffer=(unsigned char*) kalloc(file_size());
	file_read(mousebuffer, file_size);
	file_close();
}*/

inline void mouse_wait(unsigned char a_type) //unsigned char
{
  unsigned int _time_out=100000; //unsigned int
  if(a_type==0)
  {
    while(_time_out--) //Data
    {
      if((InPortByte(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {
      if((InPortByte(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

inline void mouse_write(unsigned char a_write) //unsigned char
{
  //Wait to be able to send a command
  mouse_wait(1);
  //Tell the mouse we are sending a command
  OutPortByte(0x64, 0xD4);
  //Wait for the final part
  mouse_wait(1);
  //Finally write
  OutPortByte(0x60, a_write);
}

unsigned char mouse_read()
{
  //Get's response from mouse
  mouse_wait(0);
  return InPortByte(0x60);
}

void mouse_init() {
	//int x,y;
	unsigned char status;
	/*
	mouseback=(unsigned long*) kalloc(MOUSE_SIZEX*MOUSE_SIZEY*sizeof(unsigned long));	
	for (y=0; y<MOUSE_SIZEY; y++) {
		for (x=0; x<MOUSE_SIZEX; x++) {
			mouseback[MOUSE_SIZEX*y+x] = getp(SCREENWIDTH*(y+mouse_y)+(x+mouse_x));
		}
	}
*/
 	//Enable the auxiliary mouse device
  	mouse_wait(1);
	OutPortByte(0x64, 0xA8);
 
  	//Enable the interrupts
  	mouse_wait(1);
	OutPortByte(0x64, 0x20);
  	mouse_wait(0);
  	status=(InPortByte(0x60) | 2);
  	mouse_wait(1);
	OutPortByte(0x64, 0x60);
  	mouse_wait(1);
	OutPortByte(0x60, status);
 
  	//Tell the mouse to use default settings
  	mouse_write(0xF6);
	mouse_read();  //Acknowledge
 
  	//Enable the mouse
  	mouse_write(0xF4);
  	mouse_read();  //Acknowledge	

	//mouse_mousepointer_init();	
}
