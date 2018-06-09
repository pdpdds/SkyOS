//------------------------------------------------------------------------------
#ifndef LoadEngFontH
#define LoadEngFontH
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
extern char *BitmapEngFontFilesSize;

bool IsEngFontSize(int AFileSize);
bool LoadEngFont(TEngFont *AEngFont, const char *AEngFile);
void RegisterEngFont(TEngFont *AEngFont, byte *AEngFontBuffer);
//------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif
