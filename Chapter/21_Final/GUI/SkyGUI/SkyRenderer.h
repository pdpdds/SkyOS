#pragma once

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

class SkySheet;

class SkyRenderer
{
public:
	SkyRenderer();
	virtual ~SkyRenderer();

	virtual bool Initialize() = 0;

	virtual void PutFontAscToSheet(SkySheet *sheet, int x, int y, int c, int b, char *s, int l) = 0;
	virtual void MakeTextBox(SkySheet *sht, int x0, int y0, int sx, int sy, int c) = 0;
	virtual void MakeWindow(unsigned char *buf, int xsize, int ysize, char *title, char act) = 0;
	virtual void PutFonts_ASC(char *vram, int xsize, int x, int y, char c, unsigned char *s) = 0;
	virtual void InitMouseCursor(char *mouse, char bc) = 0;
	virtual void InitScreen(unsigned char *vram, int x, int y) = 0;

	virtual void PutFont(char *vram, int xsize, int x, int y, char c, char *font) = 0;
	virtual void MakeWindowTitle(unsigned char *buf, int xsize, char *title, char act) = 0;
	virtual void BoxFill(unsigned char *vram, int xsize, int color, int x0, int y0, int x1, int y1) = 0;

};

