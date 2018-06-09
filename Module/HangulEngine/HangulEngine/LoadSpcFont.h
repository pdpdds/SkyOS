//------------------------------------------------------------------------------
#ifndef LoadSpcFontH
#define LoadSpcFontH
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
extern char *BitmapSpcFontFilesSize;

bool IsSpcFontSize(int AFileSize);
bool LoadSpcFont(TSpcFont *ASpcFont, char *ASpcFile);
bool IsSamboSpcFont(char *ASpcFile);
void RegisterSpcFont(TSpcFont *ASpcFont, byte *ASpcFontBuffer);
//------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif
