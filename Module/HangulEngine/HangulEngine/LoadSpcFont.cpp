//------------------------------------------------------------------------------
#include "hanlib.h"
#include "GetFileSize.h"
#include "LoadSpcFont.h"

char *BitmapSpcFontFilesSize = ";36096;33696;";
//------------------------------------------------------------------------------
bool IsSpcFontSize(int AFileSize)
{
    char s[20];

    sprintf(s, ";%d;", AFileSize);
    return (bool)strstr(BitmapSpcFontFilesSize, s);
}
//------------------------------------------------------------------------------
bool LoadSpcFont(TSpcFont *ASpcFont, char *ASpcFile)
{
	int FileSize, page, index;
	byte *p;
	byte* SpcFontBuffer = new byte[36096];

    FILE *fp = fopen(ASpcFile, "rb");
	if (fp == NULL)
	{
		delete SpcFontBuffer;
		return false;
	}

    FileSize = GetFileSize(fp);
	if (!IsSpcFontSize(FileSize) && (FileSize != 4096))
	{
		delete SpcFontBuffer;
		return false;
	}

	fread(SpcFontBuffer, FileSize, 1, fp);
    fclose(fp);

	if (FileSize == 4096)
		if (!IsSamboSpcFont(ASpcFile)) 
		{
			delete SpcFontBuffer;
			return false;
		}

	p = SpcFontBuffer;

    switch (FileSize) {
    case 36096:
        ASpcFont->SType = SPCFONT_KSS;
    	for (page = 0; page < 12; page++)
	    	for (index = 0; index < 94; index++, p+=32)
		    	memcpy(ASpcFont->Spc[page][index], p, 32);
        break;
    case 33696:
        ASpcFont->SType = SPCFONT_KSG;
        // 0페이지
        for (index = 0; index < 94; index++, p+=32)
            memcpy(ASpcFont->Spc[0][index], p, 32);
        // 1페이지 69개
        for (index = 0; index < 69; index++, p+=32)
            memcpy(ASpcFont->Spc[1][index], p, 32);
        // 2 ~ 4페이지
    	for (page = 2; page <= 4; page++)
	    	for (index = 0; index < 94; index++, p+=32)
		    	memcpy(ASpcFont->Spc[page][index], p, 32);
        // 5페이지 68개
        for (index = 0; index < 68; index++, p+=32)
            memcpy(ASpcFont->Spc[5][index], p, 32);
        // 6 ~ 8페이지
        for (page = 6; page <= 8; page++)
	    	for (index = 0; index < 94; index++, p+=32)
		    	memcpy(ASpcFont->Spc[page][index], p, 32);
        // 9페이지 83개
        for (index = 0; index < 83; index++, p+=32)
            memcpy(ASpcFont->Spc[9][index], p, 32);
        // 10페이지
        for (index = 0; index < 94; index++, p+=32)
            memcpy(ASpcFont->Spc[10][index], p, 32);
        // 11페이지 81개
        for (index = 0; index < 81; index++, p+=32)
            memcpy(ASpcFont->Spc[11][index], p, 32);
        break;
    case 4096:
        ASpcFont->SType = SPCFONT_SAMBO;
        for (index = 0; index < 128; index++, p+=32)
            memcpy(ASpcFont->SamboSpc[index], p, 32);
        break;
    }

	ASpcFont->FileSize = FileSize;
	ASpcFont->CharCount = FileSize / 32;
    strcpy(ASpcFont->FullFileName, ASpcFile);

	delete SpcFontBuffer;

    return true;
}
//------------------------------------------------------------------------------
bool IsSamboSpcFont(char *ASpcFile)
{
    int i;
    byte bitmap32[32];

	FILE *fp = fopen(ASpcFile, "rb");
    if (fp == NULL) return false;

	fseek(fp, 0L, SEEK_END);
    if (ftell(fp) != 4096) return false;

	fseek(fp, 0L, SEEK_SET);
    fread(bitmap32, 32, 1, fp);
    fclose(fp);

    for (i = 16; i < 32; i++)
        if (bitmap32[i] > 0) return false;

    return true;
}
//------------------------------------------------------------------------------
/*
void RegisterSpcFont(TSpcFont *ASpcFont, byte *ASpcFontBuffer)
{
	byte *p = ASpcFontBuffer;
	int page, index;
	for (page = 0; page < 12; page++)
		for (index = 0; index < 94; index++, p+=32)
			memcpy(ASpcFont->Spc[page][index], p, 32);
}
*/
//------------------------------------------------------------------------------
