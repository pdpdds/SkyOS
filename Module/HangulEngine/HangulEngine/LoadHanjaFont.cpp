//------------------------------------------------------------------------------
#include "hanlib.h"
#include "GetFileSize.h"
#include "LoadHanjaFont.h"

#define HANJAFONT_FILESIZE		156416	// == 52 * 94 * 32
#define HANJAFONT_PAGECOUNT		52
#define HANJAFONT_OFFSETCOUNT	94
#define HANJAFONT_BYTEPERCHAR	32

char *BitmapHanjaFontFilesSize = ";156416;";
//------------------------------------------------------------------------------
bool IsHanjaFontSize(int AFileSize)
{
    char s[20];

    sprintf(s, ";%d;", AFileSize);
    return (bool)strstr(BitmapHanjaFontFilesSize, s);
}
//------------------------------------------------------------------------------
bool LoadHanjaFont(THanjaFont *AHanjaFont, const char *AHanjaFile)
{
	int Page, Offset;
	byte *p;
	byte *HanjaFontFile = new byte[HANJAFONT_FILESIZE];

	FILE *fp = fopen(AHanjaFile, "rb");
	if (fp == NULL) return false;

	if (GetFileSize(fp) != HANJAFONT_FILESIZE) return false;

	fread(HanjaFontFile, HANJAFONT_FILESIZE, 1, fp);

	
    fclose(fp);

	p = HanjaFontFile;
	for (Page = 0; Page < HANJAFONT_PAGECOUNT; Page++)
    	for (Offset = 0; Offset < HANJAFONT_OFFSETCOUNT; Offset++, p+=HANJAFONT_BYTEPERCHAR)
	    	memcpy(AHanjaFont->Hanja[Page][Offset], p, HANJAFONT_BYTEPERCHAR);

	AHanjaFont->FileSize = HANJAFONT_FILESIZE;
	AHanjaFont->CharCount = HANJAFONT_FILESIZE / HANJAFONT_BYTEPERCHAR;
    strcpy(AHanjaFont->FullFileName, AHanjaFile);

	delete HanjaFontFile;

    return true;
}
//------------------------------------------------------------------------------
/*
void RegisterHanjaFont(THanjaFont *AHanjaFont, byte *AHanjaFontBuffer)
{
	int Page, Offset;
	byte *p = AHanjaFontBuffer;
	for (Page = 0; Page < HANJAFONT_PAGECOUNT; Page++)
    	for (Offset = 0; Offset < HANJAFONT_OFFSETCOUNT; Offset++, p+=HANJAFONT_BYTEPERCHAR)
	    	memcpy(AHanjaFont->Hanja[Page][Offset], p, HANJAFONT_BYTEPERCHAR);
}
*/
//------------------------------------------------------------------------------
