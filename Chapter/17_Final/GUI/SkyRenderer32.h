#pragma once
#include "SkyRenderer.h"

class SkySheet;

class SkyRenderer32 : public SkyRenderer
{
public:
	SkyRenderer32();
	virtual ~SkyRenderer32();

	virtual bool Initialize() override;

	void PutFontAscToSheet(SkySheet *sheet, int x, int y, int c, int b, char *s, int l) override;
	void MakeTextBox(SkySheet *sht, int x0, int y0, int sx, int sy, int c) override;
	void MakeWindow(unsigned char *buf, int xsize, int ysize, char *title, char act) override;
	void PutFonts_ASC(char *vram, int xsize, int x, int y, char c, unsigned char *s) override;
	void InitMouseCursor(char *mouse, char bc) override;
	bool LoadImage(unsigned char *vram, int x, int y, char* filename) override;

	void PutFont(char *vram, int xsize, int x, int y, char c, char *font) override;
	void MakeWindowTitle(unsigned char *buf, int xsize, char *title, char act) override;
	void BoxFill(unsigned char *vram, int xsize, int color, int x0, int y0, int x1, int y1) override;
};

