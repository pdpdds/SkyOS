#include "SkyOS.h"
#include "SkyRenderer8.h"
#include "SkySheet.h"
#include "JPEG.h"

extern char closebtn[14][16];
extern char cursor[16][16];
extern char skyFontData[4096];

SkyRenderer8::SkyRenderer8()
{
}


SkyRenderer8::~SkyRenderer8()
{
}

bool SkyRenderer8::Initialize()
{
	InitPalette();

	return true;
}

void SkyRenderer8::SetPalette(int start, int end, unsigned char *rgb)
{
	kEnterCriticalSection();


	for (int i = start; i <= end; i++) {
		OutPortByte(0x03c9, rgb[0] / 4);
		OutPortByte(0x03c9, rgb[1] / 4);
		OutPortByte(0x03c9, rgb[2] / 4);
		rgb += 3;
	}

	kLeaveCriticalSection();
}

void SkyRenderer8::InitPalette()
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:흑 */
		0xff, 0x00, 0x00,	/*  1:밝은 빨강 */
		0x00, 0xff, 0x00,	/*  2:밝은 초록 */
		0xff, 0xff, 0x00,	/*  3:밝은 황색 */
		0x00, 0x00, 0xff,	/*  4:밝은 파랑 */
		0xff, 0x00, 0xff,	/*  5:밝은 보라색 */
		0x00, 0xff, 0xff,	/*  6:밝은 물색 */
		0xff, 0xff, 0xff,	/*  7:흰색 */
		0xc6, 0xc6, 0xc6,	/*  8:밝은 회색 */
		0x84, 0x00, 0x00,	/*  9:어두운 빨강 */
		0x00, 0x84, 0x00,	/* 10:어두운 초록 */
		0x84, 0x84, 0x00,	/* 11:어두운 황색 */
		0x00, 0x00, 0x84,	/* 12:어두운 파랑 */
		0x84, 0x00, 0x84,	/* 13:어두운 보라색 */
		0x00, 0x84, 0x84,	/* 14:어두운 물색 */
		0x84, 0x84, 0x84	/* 15:어두운 회색 */
	};
	SetPalette(0, 15, table_rgb);
}

void SkyRenderer8::PutFonts_ASC(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	for (; *s != 0x00; s++)
	{
		if(*s == '\n')
			continue;

		PutFont(vram, xsize, x, y, c, skyFontData + *s * 16);
		x += 8;
	}
	return;
}

void SkyRenderer8::PutFont(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, d /* data */;

	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x; //문자를 출력할 버퍼 위치를 얻는다.
		d = font[i]; //폰트의 1바이트 가로데이터를 얻는다. 				 
		//1바이트의 각 비트를 테스트해서 0이 아니면 픽셀값을 써넣는다.
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}


/* 마우스 커서를 준비(16 x16) */
void SkyRenderer8::InitMouseCursor(char *mouse, char bc)
{
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

void SkyRenderer8::MakeWindow(unsigned char *buf, int xsize, int ysize, char *title, char act)
{
	BoxFill(buf, xsize, COL8_C6C6C6, 0, 0, xsize - 1, 0);
	BoxFill(buf, xsize, COL8_FFFFFF, 1, 1, xsize - 2, 1);
	BoxFill(buf, xsize, COL8_C6C6C6, 0, 0, 0, ysize - 1);
	BoxFill(buf, xsize, COL8_FFFFFF, 1, 1, 1, ysize - 2);
	BoxFill(buf, xsize, COL8_848484, xsize - 2, 1, xsize - 2, ysize - 2);
	BoxFill(buf, xsize, COL8_000000, xsize - 1, 0, xsize - 1, ysize - 1);
	BoxFill(buf, xsize, COL8_C6C6C6, 2, 2, xsize - 3, ysize - 3);
	BoxFill(buf, xsize, COL8_848484, 1, ysize - 2, xsize - 2, ysize - 2);
	BoxFill(buf, xsize, COL8_000000, 0, ysize - 1, xsize - 1, ysize - 1);
	MakeWindowTitle(buf, xsize, title, act);
	return;
}

void SkyRenderer8::MakeWindowTitle(unsigned char *buf, int xsize, char *title, char act)
{

	int x, y;
	char c, tc, tbc;
	if (act != 0) {
		tc = COL8_FFFFFF;
		tbc = COL8_000084;
	}
	else {
		tc = COL8_C6C6C6;
		tbc = COL8_848484;
	}

	BoxFill(buf, xsize, tbc, 3, 3, xsize - 4, 20);
	PutFonts_ASC((char *)buf, xsize, 24, 4, tc, (unsigned char *)title);
	for (y = 0; y < 14; y++)
	{
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL8_000000;
			}
			else if (c == '$') {
				c = COL8_848484;
			}
			else if (c == 'Q') {
				c = COL8_C6C6C6;
			}
			else {
				c = COL8_FFFFFF;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

void SkyRenderer8::PutFontAscToSheet(SkySheet *sheet, int x, int y, int c, int b, char *s, int l)
{
	unsigned char* buf = sheet->GetBuf();
	int xsize = sheet->GetXSize();

	BoxFill(buf, xsize, b, x, y, x + l * 8 - 1, y + 15);
	PutFonts_ASC((char *)buf, xsize, x, y, c, (unsigned char *)s);
	sheet->Refresh(x, y, x + l * 8, y + 16);
	return;
}

void SkyRenderer8::MakeTextBox(SkySheet *sheet, int x0, int y0, int sx, int sy, int c)
{
	unsigned char* buf = sheet->GetBuf();
	int xsize = sheet->GetXSize();
	int x1 = x0 + sx, y1 = y0 + sy;
	BoxFill(buf, xsize, COL8_848484, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
	BoxFill(buf, xsize, COL8_848484, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
	BoxFill(buf, xsize, COL8_FFFFFF, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
	BoxFill(buf, xsize, COL8_FFFFFF, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
	BoxFill(buf, xsize, COL8_000000, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
	BoxFill(buf, xsize, COL8_000000, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
	BoxFill(buf, xsize, COL8_C6C6C6, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
	BoxFill(buf, xsize, COL8_C6C6C6, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
	BoxFill(buf, xsize, c, x0 - 1, y0 - 1, x1 + 0, y1 + 0);
	return;
}

void SkyRenderer8::BoxFill(unsigned char *vram, int xsize, int color, int x0, int y0, int x1, int y1)
{
	int x, y;
	unsigned char c = (unsigned char)color;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void SkyRenderer8::InitScreen(unsigned char *vram, int x, int y)
{
	BoxFill(vram, x, COL8_008484, 0, 0, x - 1, y - 29);
	BoxFill(vram, x, COL8_C6C6C6, 0, y - 28, x - 1, y - 28);
	BoxFill(vram, x, COL8_FFFFFF, 0, y - 27, x - 1, y - 27);
	BoxFill(vram, x, COL8_C6C6C6, 0, y - 26, x - 1, y - 1);

	BoxFill(vram, x, COL8_FFFFFF, 3, y - 24, 59, y - 24);
	BoxFill(vram, x, COL8_FFFFFF, 2, y - 24, 2, y - 4);
	BoxFill(vram, x, COL8_848484, 3, y - 4, 59, y - 4);
	BoxFill(vram, x, COL8_848484, 59, y - 23, 59, y - 5);
	BoxFill(vram, x, COL8_000000, 2, y - 3, 59, y - 3);
	BoxFill(vram, x, COL8_000000, 60, y - 24, 60, y - 3);

	BoxFill(vram, x, COL8_848484, x - 47, y - 24, x - 4, y - 24);
	BoxFill(vram, x, COL8_848484, x - 47, y - 23, x - 47, y - 4);
	BoxFill(vram, x, COL8_FFFFFF, x - 47, y - 3, x - 4, y - 3);
	BoxFill(vram, x, COL8_FFFFFF, x - 3, y - 24, x - 3, y - 3);
}
