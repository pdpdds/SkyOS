//---------------------------------------------------------------------------
#ifndef LoadHanjaFontH
#define LoadHanjaFontH
//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//---------------------------------------------------------------------------
extern char *BitmapHanjaFontFilesSize;

bool IsHanjaFontSize(int AFileSize);
bool LoadHanjaFont(THanjaFont *AHanjaFont, const char *hanjafile);
void RegisterHanjaFont(THanjaFont *AHanjaFont, byte *hanjafont);
//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
#endif
