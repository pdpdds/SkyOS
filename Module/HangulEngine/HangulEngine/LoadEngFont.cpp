//-----------------------------------------------------------------------------
#include "hanlib.h"
#include "GetFileSize.h"
#include "LoadEngFont.h"
#include "SkyInterface.h"
extern SkyMockInterface g_mockInterface;

//-----------------------------------------------------------------------------
char *BitmapEngFontFilesSize =
    ";" "2048"
    ";" "4096"
    ";" "4112"
    ";";
//-----------------------------------------------------------------------------
bool IsEngFontSize(int AFileSize)
{
    char s[20];

    sprintf(s, ";%d;", AFileSize);
    return (bool)strstr(BitmapEngFontFilesSize, s);
}

//-----------------------------------------------------------------------------
bool LoadEngFont(TEngFont *AEngFont, const char *AEngFile)
{
	int i, size;
    FILE *fp = fopen(AEngFile, "rb");
	if (fp == NULL) return false;

	size = GetFileSize(fp);
	//g_mockInterface.g_printInterface.sky_printf("%d\n", size);
	
	switch (size) {
	case 2048: 
	case 4096: 
	case 4112: break;
	default: 
		return false;
	}

	

	for (i = 0; i < 128; i++)
		fread(AEngFont->Eng[i], 16, 1, fp);

	switch (size) {
	case 2048:
		AEngFont->CharCount = 128;
		break;
	case 4096:
		AEngFont->CharCount = 256;
    	for (i = 128; i < 256; i++)
			fread(AEngFont->Eng[i], 16, 1, fp);
		break;
	case 4112:
		AEngFont->CharCount = 256;
    	for (i = 128; i < 128 + 18; i++) 
			fread(AEngFont->Eng[i], 16, 1, fp);
		fseek(fp, 16, SEEK_CUR);
    	for (i = 128 + 18; i < 256; i++)
			fread(AEngFont->Eng[i], 16, 1, fp);
		break;
	}
    
	fclose(fp);

	AEngFont->FileSize = size;
    strcpy(AEngFont->FullFileName, AEngFile);
	
    return true;
}
//-----------------------------------------------------------------------------
/*
void RegisterEngFont(TEngFont *AEngFont, byte *AEngFontBuffer)
{
	int i;
	byte *p = AEngFontBuffer;
	for (i = 0; i < 128; i++, p+=16) 
		memcpy(AEngFont->Eng[i], p, 16);
}
*/
//-----------------------------------------------------------------------------
