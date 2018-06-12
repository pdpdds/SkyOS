#include "HangulEngine.h"

#include "hanlib.h"      // 한글 라이브러리 0.17
#include "hancode\hancode.h"    // 한라프로 3.0 한글코드 변환 라이브러리 0.1
#include "hancode\hanin.h"      // "

#include "SkyInterface.h"
extern SkyMockInterface g_mockInterface;

TEngFont *pDefEngFont = NULL;
THanFont *pDefHanFont = NULL;
TSpcFont *pDefSpcFont = NULL;
THanjaFont *pDefHanjaFont = NULL;

TEngFont EngFont1;
THanFont HanFont1;
TSpcFont SpcFont1;
THanjaFont HanjaFont1;

HangulEngine::HangulEngine()
{
}


HangulEngine::~HangulEngine()
{
}

bool HangulEngine::Initialize()
{
	char *EngFontFile = "enround.fnt";
	char *HanFontFile = "hanround.fnt";
	char *SpcFontFile = "kss1.fnt";
	char *HanjaFontFile = "hanja.fnt";

	if (!LoadEngFont(&EngFont1, EngFontFile))
	{
		g_mockInterface.g_printInterface.sky_printf(" can't find! %s\n", EngFontFile);
		return false;
	}
	else pDefEngFont = &EngFont1;

	if (!LoadHanFont(&HanFont1, HanFontFile))
	{
		g_mockInterface.g_printInterface.sky_printf(" can't find! %s\n", HanFontFile);
		return false;
	}
	else pDefHanFont = &HanFont1;
	
	if (!LoadSpcFont(&SpcFont1, SpcFontFile))
	{
		g_mockInterface.g_printInterface.sky_printf(" can't find! %s\n", SpcFontFile);
		return false;
	}
	else pDefSpcFont = &SpcFont1;
	
	if (!LoadHanjaFont(&HanjaFont1, HanjaFontFile))
	{
		g_mockInterface.g_printInterface.sky_printf(" can't find! %s\n", HanjaFontFile);
		return false;
	}
	else pDefHanjaFont = &HanjaFont1;


	return true;

	//HanTextOut(0, 0, (byte*)"한글 테a스트");
}

void HangulEngine::PutFonts(char *vram, int xsize, int x, int y, unsigned int c, unsigned char *s)
{
	byte bitmap32[32], johab[2];

	for (unsigned i = 0; i < strlen((char*)s); ) {
		if (ishangul1st(s, i)) {
			// CP949 확장완성형 → 상용 조합형으로 변환
			code_cnvtcs(CODE_KSSM, CODE_CP949, johab, s[i], s[i + 1]);

			codeidx_t codeidx;
			_code->getidx(&codeidx, johab[0], johab[1]);
			switch (codeidx.kind) {
			case CODEKIND_SPECIAL:
				PutFont16x16(vram, xsize, x, y, c, (char*)(pDefSpcFont->Spc[s[i] - 0xA1][s[i + 1] - 0xA1]));
				break;
			case CODEKIND_HANGUL:
				CompleteHanChar(bitmap32, johab, pDefHanFont);
				PutFont16x16(vram, xsize, x, y, c, (char*)bitmap32);
				break;
			case CODEKIND_HANJA:
				PutFont16x16(vram, xsize, x, y, c, (char*)(pDefHanjaFont->Hanja[s[i] - 0xCA][s[i + 1] - 0xA1]));
				break;
			}
			x += 16, i += 2;
		}
		else {
			PutFont8x16(vram, xsize, x, y, c, (char*)(pDefEngFont->Eng[s[i]]));
			x += 8, i++;
		}
	}
}

void HangulEngine::PutFont16x16(char *vram, int xsize, int x, int y, unsigned int c, char *font)
{
	int i;
	ULONG *p;
	char d[2];

	for (i = 0; i < 16; i++) {
		p = ((ULONG *)vram) + (y + i) * xsize + x;
		d[0] = ~font[2 * i];
		d[1] = ~font[2 * i + 1];

		for (int j = 0; j < 2; j++)
		{
			if ((d[j] & 0x80) == 0) { p[0 + j*8] = c; }
			if ((d[j] & 0x40) == 0) { p[1 + j * 8] = c; }
			if ((d[j] & 0x20) == 0) { p[2 + j * 8] = c; }
			if ((d[j] & 0x10) == 0) { p[3 + j * 8] = c; }
			if ((d[j] & 0x08) == 0) { p[4 + j * 8] = c; }
			if ((d[j] & 0x04) == 0) { p[5 + j * 8] = c; }
			if ((d[j] & 0x02) == 0) { p[6 + j * 8] = c; }
			if ((d[j] & 0x01) == 0) { p[7 + j * 8] = c; }
		}
		
	}
	return;
}

void HangulEngine::PutFont8x16(char *vram, int xsize, int x, int y, unsigned int c, char *font)
{
	int i;
	ULONG *p;
	char d;

	for (i = 0; i < 16; i++) 
	{
		p = ((ULONG *)vram) + (y + i) * xsize + x;
		d = ~font[i];
		
		if ((d & 0x80) == 0) { p[0] = c; }
		if ((d & 0x40) == 0) { p[1] = c; }
		if ((d & 0x20) == 0) { p[2] = c; }
		if ((d & 0x10) == 0) { p[3] = c; }
		if ((d & 0x08) == 0) { p[4] = c; }
		if ((d & 0x04) == 0) { p[5] = c; }
		if ((d & 0x02) == 0) { p[6] = c; }
		if ((d & 0x01) == 0) { p[7] = c; }
	}
	return;
}

void HangulEngine::CompleteHanChar(unsigned char* ABuffer32, unsigned char* AHanByte, THanFont *AHanFont)
{
	THangul _Hangul;
	bool flag = true;

	_Hangul.HanByte.Byte0 = AHanByte[0];
	_Hangul.HanByte.Byte1 = AHanByte[1];

	int F1 = _CodeTable[0][_Hangul.HanCode.F1];
	int F2 = _CodeTable[1][_Hangul.HanCode.F2];
	int F3 = _CodeTable[2][_Hangul.HanCode.F3];

	int F3B = AHanFont->pF3B[F2];
	int F2B = AHanFont->pF2B[F1 * 2 + (F3 != 0)];
	int F1B = AHanFont->pF1B[F2 * 2 + (F3 != 0)];

	if (F1) HanComplete(true, ABuffer32, AHanFont->F1[F1B][F1], 32), flag = false;
	if (F2) HanComplete(flag, ABuffer32, AHanFont->F2[F2B][F2], 32), flag = false;
	if (F3)	HanComplete(flag, ABuffer32, AHanFont->F3[F3B][F3], 32), flag = false;
}