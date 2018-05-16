#include "SkyOS.h"
#include "SkyRenderer32.h"
#include "SkySheet.h"
#include "JPEG.h"

extern char closebtn[14][16];
extern char cursor[16][16];
extern char skyFontData[4096];

SkyRenderer32::SkyRenderer32()
{
}


SkyRenderer32::~SkyRenderer32()
{
}

bool SkyRenderer32::Initialize()
{
	return true;
}

void SkyRenderer32::PutFonts_ASC(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	for (; *s != 0x00; s++)
	{
		if (*s == '\n')
			continue;

		PutFont(vram, xsize, x, y, c, skyFontData + *s * 16);
		x += 8;
	}
	return;
}

void SkyRenderer32::PutFont(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	ULONG *p;
	char d;

	for (i = 0; i < 16; i++) {
		p = ((ULONG *)vram) + (y + i) * xsize + x;
		d = font[i];
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
void SkyRenderer32::InitMouseCursor(char *mouse, char bc)
{
	int x, y;
	ULONG* pMemory = (ULONG*)mouse;
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				pMemory[y * 16 + x] = 0x00000000;
			}
			if (cursor[y][x] == 'O') {
				pMemory[y * 16 + x] = 0x00FFFFFF;
			}
			if (cursor[y][x] == '.') {
				pMemory[y * 16 + x] = 0xFFFFFFFF;
			}
		}
	}
	return;
}

void SkyRenderer32::MakeWindow(unsigned char *buf, int xsize, int ysize, char *title, char act)
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

void SkyRenderer32::MakeWindowTitle(unsigned char *buf, int xsize, char *title, char act)
{

	int x, y;
	ULONG* pMemory = (ULONG*)buf;
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
			pMemory[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

void SkyRenderer32::PutFontAscToSheet(SkySheet *sheet, int x, int y, int c, int b, char *s, int l)
{
	unsigned char* buf = sheet->GetBuf();
	int xsize = sheet->GetXSize();

	BoxFill(buf, xsize, b, x, y, x + l * 8 - 1, y + 15);
	PutFonts_ASC((char *)buf, xsize, x, y, c, (unsigned char *)s);
	sheet->Refresh(x, y, x + l * 8, y + 16);
}

void SkyRenderer32::MakeTextBox(SkySheet *sheet, int x0, int y0, int sx, int sy, int c)
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
}

void SkyRenderer32::BoxFill(unsigned char *vram, int xsize, int color, int x0, int y0, int x1, int y1)
{
	int x, y;
	ULONG* pMemory = (ULONG*)vram;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			pMemory[y * xsize + x] = color;
	}
	return;
}

void SkyRenderer32::InitScreen(unsigned char *vram, int x, int y)
{
	ULONG* pBuffer = (ULONG*)vram;
	FILE* pFile = StorageManager::GetInstance()->OpenFile("Background.jpg", "rb");

	if (pFile != nullptr)
	{
		JPEG* jpeg = new JPEG;

		char* buffer = new char[pFile->_fileLength];

		StorageManager::GetInstance()->ReadFile(pFile, (unsigned char*)buffer, 1, pFile->_fileLength);

		if (true == kJPEGInit(jpeg, (BYTE*)buffer, pFile->_fileLength))
		{
			// 디코딩할 메모리 할당
			COLOR* outBuffer = (COLOR*) new char[jpeg->width * jpeg->height * sizeof(COLOR)];
			ULONG* rgb888Buffer = (ULONG*) new char[jpeg->width * jpeg->height * sizeof(ULONG)];

			if (outBuffer != nullptr)
			{
				if (true == kJPEGDecode(jpeg, outBuffer))
				{
					for (int i = 0; i < jpeg->width * jpeg->height; i++)
					{
						COLOR color = outBuffer[i];
						uint8_t r = ((color >> 11) & 0x1F);
						uint8_t g = ((color >> 5) & 0x3F);
						uint8_t b = (color & 0x1F);

						r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
						g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
						b = (((color & 0x1F) * 527) + 23) >> 6;

						uint32_t RGB888 = r << 16 | g << 8 | b;

						rgb888Buffer[i] = RGB888;
					}
					
					for (int y = 0; y < jpeg->height; ++y)
					{
						for (int x = 0; x < jpeg->width; ++x)
						{
							pBuffer[y * jpeg->width + x] = rgb888Buffer[y * jpeg->width + x]; //노란색
						}
					}
				}
			}
		}
	}
	else
	{
		BoxFill(vram, x, 0x00FF0000, 0, 0, x - 1, y - 29);
	}

	BoxFill(vram, x, 0x0000FF00, 0, y - 28, x - 1, y - 28);
	BoxFill(vram, x, 0x000000FF, 0, y - 27, x - 1, y - 27);
	BoxFill(vram, x, 0x0000FF00, 0, y - 26, x - 1, y - 1);

	BoxFill(vram, x, 0x000000FF, 3, y - 24, 59, y - 24);
	BoxFill(vram, x, 0x000000FF, 2, y - 24, 2, y - 4);
	BoxFill(vram, x, 0x00FF0000, 3, y - 4, 59, y - 4);
	BoxFill(vram, x, 0x00FF0000, 59, y - 23, 59, y - 5);
	BoxFill(vram, x, 0x00000000, 2, y - 3, 59, y - 3);
	BoxFill(vram, x, 0x00000000, 60, y - 24, 60, y - 3);

	BoxFill(vram, x, 0x00FF0000, x - 47, y - 24, x - 4, y - 24);
	BoxFill(vram, x, 0x00FF0000, x - 47, y - 23, x - 47, y - 4);
	BoxFill(vram, x, 0x000000FF, x - 47, y - 3, x - 4, y - 3);
	BoxFill(vram, x, 0x000000FF, x - 3, y - 24, x - 3, y - 3);
}

