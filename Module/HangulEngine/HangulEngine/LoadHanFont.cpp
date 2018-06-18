//------------------------------------------------------------------------------
#include "hanlib.h"
#include "LoadHanFont.h"
#include "Table10x4x4.h"
#include "Table8x4x4.h"
#include "Table6x2x1.h"
#include "Table2x1x2.h"

#define	F1COUNT	19	// 초성 개수
#define	F2COUNT	21	// 중성 개수
#define	F3COUNT	27	// 종성 개수
//------------------------------------------------------------------------------
char *BitmapHanFontFilesSize =
    ";" "3616"  // 2x1x2 (한라프로)
    ";" "3776"  // 2x1x2
    ";" "6176"  // 6x2x1
    ";" "11008" // 8x4x4
    ";" "11520" // 8x4x4
    ";" "13056" // 8x4x4
    ";" "12224" // 10x4x4 (한라프로)
    ";" "12800" // 10x4x4 또는 8x4x4
    ";";
//------------------------------------------------------------------------------
bool IsHanFontSize(int AFileSize)
{
    char s[20];

    sprintf(s, ";%d;", AFileSize);
    return (bool)strstr(BitmapHanFontFilesSize, s);
}
//------------------------------------------------------------------------------
static int GetFileSize(char *AFileName)
{
	int length;

	FILE *fp = fopen(AFileName, "rb");
    if (fp == NULL) return 0;

	fseek(fp, 0L, SEEK_END);
	length = ftell(fp);
    fclose(fp);

	return length;
}
//------------------------------------------------------------------------------
bool LoadHanFont(THanFont *AHanFont, char *AHanFile)
{
	byte* Buffer = new byte[13056];
    byte *p = Buffer;
	int i, j, FileSize;
    FILE *fp;

    FileSize = GetFileSize(AHanFile);
    switch (FileSize) {
	case 11008: // 8x4x4 한라프로
		AHanFont->HanFontType = HANFONT_8X4X4;
		AHanFont->F1BulCount = 8;
		AHanFont->F2BulCount = 4;
        AHanFont->F3BulCount = 4;
    	AHanFont->F_SKIP = 0;
		AHanFont->F1_SKIP = AHanFont->F2_SKIP = AHanFont->F3_SKIP = 0;

        AHanFont->pF1B = _F1B_8x4x4;
    	AHanFont->pF2B = _F2B_8x4x4;
    	AHanFont->pF3B = _F3B_8x4x4;
        break;
	case 11520: // 8x4x4 도깨비
		AHanFont->HanFontType = HANFONT_8X4X4;
		AHanFont->F1BulCount = 8;
		AHanFont->F2BulCount = 4;
		AHanFont->F3BulCount = 4;
    	AHanFont->F_SKIP = 1;
		AHanFont->F1_SKIP = AHanFont->F2_SKIP = AHanFont->F3_SKIP = 0;

        AHanFont->pF1B = _F1B_8x4x4;
    	AHanFont->pF2B = _F2B_8x4x4;
    	AHanFont->pF3B = _F3B_8x4x4;
        break;
    case 12224: // 10x4x4 한라프로
        AHanFont->HanFontType = HANFONT_10X4X4;
        AHanFont->F1BulCount = 10;
        AHanFont->F2BulCount = 4;
        AHanFont->F3BulCount = 4;
        AHanFont->F_SKIP = 0;
        AHanFont->F1_SKIP = AHanFont->F2_SKIP = AHanFont->F3_SKIP = 0;

        AHanFont->pF1B = _F1B_10x4x4;
    	AHanFont->pF2B = _F2B_10x4x4;
    	AHanFont->pF3B = _F3B_10x4x4;
        break;
    case 13056: p += 256;   // 8x4x4 이야기 6.1 이상
    case 12800: // 8x4x4 이야기 6.0 또는 10x4x4 조합 글꼴
        if (IsHanFont10x4x4(AHanFile)) {
            AHanFont->HanFontType = HANFONT_10X4X4;
            AHanFont->F1BulCount = 10;
            AHanFont->F2BulCount = 4;
            AHanFont->F3BulCount = 4;
            AHanFont->F_SKIP = 1;
            AHanFont->F1_SKIP = AHanFont->F2_SKIP = AHanFont->F3_SKIP = 0;

            AHanFont->pF1B = _F1B_10x4x4;
        	AHanFont->pF2B = _F2B_10x4x4;
        	AHanFont->pF3B = _F3B_10x4x4;
        } else {
            AHanFont->HanFontType = HANFONT_8X4X4;
            AHanFont->F1BulCount = 8;
            AHanFont->F2BulCount = 4;
            AHanFont->F3BulCount = 4;
            AHanFont->F_SKIP = 0;
            AHanFont->F1_SKIP = 4;
            AHanFont->F2_SKIP = 2;
            AHanFont->F3_SKIP = 4;

            AHanFont->pF1B = _F1B_8x4x4;
        	AHanFont->pF2B = _F2B_8x4x4;
        	AHanFont->pF3B = _F3B_8x4x4;
        }
        break;
	case 6176:	// 6x2x1
		AHanFont->HanFontType = HANFONT_6X2X1;
		AHanFont->F1BulCount = 6;
		AHanFont->F2BulCount = 2;
		AHanFont->F3BulCount = 1;
    	AHanFont->F_SKIP = 1;
		AHanFont->F1_SKIP = AHanFont->F2_SKIP = AHanFont->F3_SKIP = 0;

        AHanFont->pF1B = _F1B_6x2x1;
    	AHanFont->pF2B = _F2B_6x2x1;
    	AHanFont->pF3B = _F3B_6x2x1;
		break;
    case 3616:  // 2x1x2 (한라프로 글꼴)
		AHanFont->HanFontType = HANFONT_2X1X2;
		AHanFont->F1BulCount = 2;
		AHanFont->F2BulCount = 1;
		AHanFont->F3BulCount = 2;
        AHanFont->F_SKIP = 0;
		AHanFont->F1_SKIP = AHanFont->F2_SKIP = AHanFont->F3_SKIP = 0;

        AHanFont->pF1B = _F1B_2x1x2;
    	AHanFont->pF2B = _F2B_2x1x2;
    	AHanFont->pF3B = _F3B_2x1x2;
		break;
	case 3776:	// 2x1x2
		AHanFont->HanFontType = HANFONT_2X1X2;
		AHanFont->F1BulCount = 2;
		AHanFont->F2BulCount = 1;
		AHanFont->F3BulCount = 2;
        AHanFont->F_SKIP = 1;
		AHanFont->F1_SKIP = AHanFont->F2_SKIP = AHanFont->F3_SKIP = 0;

        AHanFont->pF1B = _F1B_2x1x2;
    	AHanFont->pF2B = _F2B_2x1x2;
    	AHanFont->pF3B = _F3B_2x1x2;
		break;
    default: 
		delete Buffer;
		return false;
    }

	fp = fopen(AHanFile, "rb");
	fread(Buffer, FileSize, 1, fp);
    fclose(fp);

    AHanFont->F1Count = AHanFont->F_SKIP + F1COUNT + AHanFont->F1_SKIP;
    AHanFont->F2Count = AHanFont->F_SKIP + F2COUNT + AHanFont->F2_SKIP;
    AHanFont->F3Count = AHanFont->F_SKIP + F3COUNT + AHanFont->F3_SKIP;

	AHanFont->CharsCount = (AHanFont->F1Count * AHanFont->F1BulCount)
						 + (AHanFont->F2Count * AHanFont->F2BulCount)
						 + (AHanFont->F3Count * AHanFont->F3BulCount);

	for (i = 0; i < AHanFont->F1BulCount; i++)
		for (j = 0; j < AHanFont->F1Count; j++, p+=32)
			memcpy(AHanFont->F1[i][!AHanFont->F_SKIP + j], p, 32);
	for (i = 0; i < AHanFont->F2BulCount; i++)
        for (j = 0; j < AHanFont->F2Count; j++, p+=32)
        	memcpy(AHanFont->F2[i][!AHanFont->F_SKIP + j], p, 32);
	for (i = 0; i < AHanFont->F3BulCount; i++)
        for (j = 0; j < AHanFont->F3Count; j++, p+=32)
        	memcpy(AHanFont->F3[i][!AHanFont->F_SKIP + j], p, 32);

    AHanFont->FileSize = FileSize;
    strcpy(AHanFont->FullFileName, AHanFile);

	GetHangulJamo(AHanFont);
	delete Buffer;
	return true;
}
//------------------------------------------------------------------------------
void GetHangulJamo(THanFont *AHanFont)
{
	AHanFont->pHangulJamo[ 0] = &(AHanFont->F1[0][ 1][0]);		// 'ㄱ'	(초성, 종성)		: 0xA4A1
	AHanFont->pHangulJamo[ 1] = &(AHanFont->F1[0][ 2][0]);		// 'ㄲ'	(복초성, 복종성)	: 0xA4A2
	AHanFont->pHangulJamo[ 2] = &(AHanFont->F3[0][ 3][0]);		// 'ㄳ'	(복종성)			: 0xA4A3
	AHanFont->pHangulJamo[ 3] = &(AHanFont->F1[0][ 3][0]);		// 'ㄴ'	(초성, 종성)		: 0xA4A4
	AHanFont->pHangulJamo[ 4] = &(AHanFont->F3[0][ 5][0]);		// 'ㄵ'	(복종성)			: 0xA4A5
	AHanFont->pHangulJamo[ 5] = &(AHanFont->F3[0][ 6][0]);		// 'ㄶ'	(복종성)			: 0xA4A6
	AHanFont->pHangulJamo[ 6] = &(AHanFont->F1[0][ 4][0]);		// 'ㄷ'	(초성, 종성)		: 0xA4A7
	AHanFont->pHangulJamo[ 7] = &(AHanFont->F1[0][ 5][0]);		// 'ㄸ'	(복초성)			: 0xA4A8
	AHanFont->pHangulJamo[ 8] = &(AHanFont->F1[0][ 6][0]);		// 'ㄹ'	(초성, 종성)		: 0xA4A9
	AHanFont->pHangulJamo[ 9] = &(AHanFont->F3[0][ 9][0]);		// 'ㄺ'	(복종성)			: 0xA4AA
	AHanFont->pHangulJamo[10] = &(AHanFont->F3[0][10][0]);		// 'ㄻ'	(복종성)            : 0xA4AB
	AHanFont->pHangulJamo[11] = &(AHanFont->F3[0][11][0]);		// 'ㄼ'	(복종성)            : 0xA4AC
	AHanFont->pHangulJamo[12] = &(AHanFont->F3[0][12][0]);		// 'ㄽ'	(복종성)            : 0xA4AD
	AHanFont->pHangulJamo[13] = &(AHanFont->F3[0][13][0]);		// 'ㄾ'	(복종성)            : 0xA4AE
	AHanFont->pHangulJamo[14] = &(AHanFont->F3[0][14][0]);		// 'ㄿ'	(복종성)            : 0xA4AF
	AHanFont->pHangulJamo[15] = &(AHanFont->F3[0][15][0]);		// 'ㅀ'	(복종성)            : 0xA4B0
	AHanFont->pHangulJamo[16] = &(AHanFont->F1[0][ 7][0]);		// 'ㅁ'	(초성, 종성)        : 0xA4B1
	AHanFont->pHangulJamo[17] = &(AHanFont->F1[0][ 8][0]);		// 'ㅂ'	(초성, 종성)        : 0xA4B2
	AHanFont->pHangulJamo[18] = &(AHanFont->F1[0][ 9][0]);		// 'ㅃ'	(복초성)			: 0xA4B3
	// 0.52 수정 - 바로 아래줄의 [16] -> [18]로 수정함.
	AHanFont->pHangulJamo[19] = &(AHanFont->F3[0][18][0]);		// 'ㅄ'	(복종성)            : 0xA4B4
	AHanFont->pHangulJamo[20] = &(AHanFont->F1[0][10][0]);		// 'ㅅ'	(초성, 종성)        : 0xA4B5
	AHanFont->pHangulJamo[21] = &(AHanFont->F1[0][11][0]);		// 'ㅆ'	(복초성, 복종성)    : 0xA4B6
	AHanFont->pHangulJamo[22] = &(AHanFont->F1[0][12][0]);		// 'ㅇ'	(초성, 종성)        : 0xA4B7
	AHanFont->pHangulJamo[23] = &(AHanFont->F1[0][13][0]);		// 'ㅈ'	(초성, 종성)        : 0xA4B8
	AHanFont->pHangulJamo[24] = &(AHanFont->F1[0][14][0]);		// 'ㅉ'	(복초성)            : 0xA4B9
	AHanFont->pHangulJamo[25] = &(AHanFont->F1[0][15][0]);		// 'ㅊ'	(초성, 종성)        : 0xA4BA
	AHanFont->pHangulJamo[26] = &(AHanFont->F1[0][16][0]);		// 'ㅋ'	(초성, 종성)        : 0xA4BB
	AHanFont->pHangulJamo[27] = &(AHanFont->F1[0][17][0]);		// 'ㅌ'	(초성, 종성)		: 0xA4BC
	AHanFont->pHangulJamo[28] = &(AHanFont->F1[0][18][0]);		// 'ㅍ'	(초성, 종성)        : 0xA4BD
	AHanFont->pHangulJamo[29] = &(AHanFont->F1[0][19][0]);		// 'ㅎ'	(초성, 종성)        : 0xA4BE

	AHanFont->pHangulJamo[30] = &(AHanFont->F2[0][ 1][0]);		// 'ㅏ'		: 0xA4BF
	AHanFont->pHangulJamo[31] = &(AHanFont->F2[0][ 2][0]);		// 'ㅐ'		: 0xA4C0
	AHanFont->pHangulJamo[32] = &(AHanFont->F2[0][ 3][0]);		// 'ㅑ'		: 0xA4C1
	AHanFont->pHangulJamo[33] = &(AHanFont->F2[0][ 4][0]);		// 'ㅒ'		: 0xA4C2
	AHanFont->pHangulJamo[34] = &(AHanFont->F2[0][ 5][0]);		// 'ㅓ'		: 0xA4C3
	AHanFont->pHangulJamo[35] = &(AHanFont->F2[0][ 6][0]);		// 'ㅔ'		: 0xA4C4
	AHanFont->pHangulJamo[36] = &(AHanFont->F2[0][ 7][0]);		// 'ㅕ'		: 0xA4C5
	AHanFont->pHangulJamo[37] = &(AHanFont->F2[0][ 8][0]);		// 'ㅖ'		: 0xA4C6
	AHanFont->pHangulJamo[38] = &(AHanFont->F2[0][ 9][0]);		// 'ㅗ'		: 0xA4C7
	AHanFont->pHangulJamo[39] = &(AHanFont->F2[0][10][0]);		// 'ㅘ'		: 0xA4C8
	AHanFont->pHangulJamo[40] = &(AHanFont->F2[0][11][0]);		// 'ㅙ'		: 0xA4C9
	AHanFont->pHangulJamo[41] = &(AHanFont->F2[0][12][0]);		// 'ㅚ'		: 0xA4CA
	AHanFont->pHangulJamo[42] = &(AHanFont->F2[0][13][0]);		// 'ㅛ'		: 0xA4CB
	AHanFont->pHangulJamo[43] = &(AHanFont->F2[0][14][0]);		// 'ㅜ'		: 0xA4CC
	AHanFont->pHangulJamo[44] = &(AHanFont->F2[0][15][0]);		// 'ㅝ'		: 0xA4CD
	AHanFont->pHangulJamo[45] = &(AHanFont->F2[0][16][0]);		// 'ㅞ'		: 0xA4CE
	AHanFont->pHangulJamo[46] = &(AHanFont->F2[0][17][0]);		// 'ㅟ'		: 0xA4CF
	AHanFont->pHangulJamo[47] = &(AHanFont->F2[0][18][0]);		// 'ㅠ'		: 0xA4D0
	AHanFont->pHangulJamo[48] = &(AHanFont->F2[0][19][0]);		// 'ㅡ'		: 0xA4D1
	AHanFont->pHangulJamo[49] = &(AHanFont->F2[0][20][0]);		// 'ㅢ'		: 0xA4D2
	AHanFont->pHangulJamo[50] = &(AHanFont->F2[0][21][0]);		// 'ㅣ'		: 0xA4D3
}
//------------------------------------------------------------------------------
bool IsHanFont10x4x4(char *AFileName)
{
    int i;
    byte bitmap32[32];

	FILE *fp = fopen(AFileName, "rb");
    if (fp == NULL) return false;

	fseek(fp, 0L, SEEK_END);
    if (ftell(fp) != 12800) return false;

	fseek(fp, 0L, SEEK_SET);
    fread(bitmap32, 32, 1, fp);
    fclose(fp);

    for (i = 0; i < 32; i++)
        if (bitmap32[i] > 0) return false;  // 이야기 6.0 글꼴

    return true;
}
//------------------------------------------------------------------------------
