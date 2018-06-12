#pragma once
#include "I_Hangul.h"
#include "hanlib.h"

class HangulEngine : public I_Hangul
{
public:
	HangulEngine();
	~HangulEngine();

	bool Initialize() override;
	void PutFonts(char *vram, int xsize, int x, int y, unsigned int c, unsigned char *s) override;

private:
	void CompleteHanChar(unsigned char* ABuffer32, unsigned char* AHanByte, THanFont *AHanFont);
	void PutFont8x16(char *vram, int xsize, int x, int y, unsigned int c, char *font);
	void PutFont16x16(char *vram, int xsize, int x, int y, unsigned int c, char *font);
};

