#include "windef.h"
//#include "atapi.h"
//#include "fat32.h"
#include "font.h"
#include "fontres.h"
#include "string.h"
#include "fileio.h"


unsigned char *fontx;
unsigned char alpha[]=" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					  "[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~";
//$$$ x-size= 16

#define SCREENWIDTH 1024
#define SCREENHEIGHT  768

#define testbit(addr, bit) ((addr) & (1 << (bit)))

void __putchar(char c, int x2, int y2, unsigned long *offscreen) {
	int i,y,x,j,k;
	unsigned int r,g,b;

	for (i=0; i<strlen((const char*)alpha); i++)
		if (alpha[i] == c) break;
	
	j=0;
	for (y=0; y<FONT_HEIGHT; y++) {
		if (y+y2 > SCREENWIDTH || y+y2<0) continue;
		j=(FONTPICTURE_WIDTH*(y+(i/16)*(FONT_HEIGHT-1)))*3;
		for (x=0; x<FONT_WIDTH; x++) {
			if (x+x2 > SCREENWIDTH || x+x2<0) continue;
			r=fontx[j+(((FONT_WIDTH*i+i/16))*3)];
			g=fontx[j+(((FONT_WIDTH*i+i/16))*3)+1];
			b=fontx[j+(((FONT_WIDTH*i+i/16))*3)+2];
			
			j+=3;
			//if ((r==0) && (g==0) && (b==0))
				offscreen[SCREENWIDTH*(y+y2)+(x+x2)] = (r<<16) | (g<<8) | b;
		}	
	}
}

void putstring(char *s, int x, int y, unsigned long *offscreen) {
	int i;
	
	for (i=0; i<strlen(s); i++) {
		__putchar(s[i], x+(i*FONT_WIDTH), y, offscreen);
	}	
}


void putcharc(char c, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned long *offscreen) {
	int i,y,x,j,k;
	unsigned int r2,g2,b2;
	
	for (i=0; i<strlen((const char*)alpha); i++)
		if (alpha[i] == c) break;
	
	j=0;
	for (y=0; y<FONT_HEIGHT; y++) {
		if (y+y2 > SCREENWIDTH || y+y2<0) continue;
		j=(FONTPICTURE_WIDTH*(y+(i/16)*(FONT_HEIGHT-1)))*3;
		for (x=0; x<FONT_WIDTH; x++) {
			if (x+x2 > SCREENWIDTH || x+x2<0) continue;
			r2=fontx[j+(((FONT_WIDTH*i+i/16))*3)];
			g2=fontx[j+(((FONT_WIDTH*i+i/16))*3)+1];
			b2=fontx[j+(((FONT_WIDTH*i+i/16))*3)+2];
			
			j+=3;
			if ((r2==0) && (g2==0) && (b2==0))
				offscreen[SCREENWIDTH*(y+y2)+(x+x2)] = (r<<16) | (g<<8) | b;
		}	
	}
}

void putstringc(char *s, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned long *offscreen) {
	int i,k;
	
	for (i=0,k=0; i<strlen(s); i++,k++) {
		if (s[i] == '\n') {
			y+=FONT_HEIGHT;
			k=-1;
			continue;
		}
		putcharc(s[i], x+(k*FONT_WIDTH), y, r, g, b, offscreen);
	}	
}

void font_init() {

	FILE* fp = fopen("font.raw", "rb");
	
	if (fp == nullptr) {
		//HaltSystem("Could not read from device");	
		return;
	}

	fseek(fp, 0, SEEK_END);
	int fileLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	;
	fontx=(unsigned char*) new char[fileLength];
	fread(fontx, fp->_fileLength, 1, fp);
	fclose(fp);
}

