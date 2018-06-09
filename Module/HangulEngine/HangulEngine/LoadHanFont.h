//------------------------------------------------------------------------------
#ifndef LoadHanFontH
#define LoadHanFontH
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
extern char *BitmapHanFontFilesSize;

bool IsHanFontSize(int AFileSize);
bool LoadHanFont(THanFont *AHanFont, char *hanfile);
void GetHangulJamo(THanFont *AHanFont);
bool IsHanFont10x4x4(char *AFileName);
//------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif
