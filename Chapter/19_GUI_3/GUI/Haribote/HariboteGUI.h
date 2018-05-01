#pragma once
#include "sheet.h"
#include "stdint.h"
#include "mouse.h"
#include "fifo.h"

#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

class HariboteGUI
{
public:
	HariboteGUI();
	~HariboteGUI();
	
	bool Init(char* vram, int screenX, int screenY);
	bool Run();
	void TestRun(uint8_t* dmaVideo);

	SHEET* GetConsoleSheet() { return sht_cons; }
	FIFO32* GetConsoleFifo() { return &consoleFifo; }

	void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
	void init_screen8(unsigned char *vram, int x, int y);
	void set_palette(int start, int end, unsigned char *rgb);
	void InitPalette();
	void init_mouse_cursor8(char *mouse, char bc);
	void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
	void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);

	void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);
	void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);
	void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
	void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);


private:
	struct SHTCTL *shtctl;
	unsigned char *buf_back;
	struct SHEET *sht_back;
	unsigned char buf_mouse[256];

	unsigned char *buf_win;
	struct SHEET *sht_win;
	struct SHEET *sht_mouse;

	int mx; 
	int  my; 
	int  cursor_x;
	int  cursor_c;

	MOUSE_DEC mdec;
	FIFO32 fifo;
	FIFO32 keycmd;
	int fifobuf[128];
	int keycmd_buf[32];

	int m_screenX;
	int m_screenY;
	char* m_vram;

	unsigned char* buf_cons;
	SHEET *sht_cons;
	FIFO32 consoleFifo;
};

