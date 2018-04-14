#pragma once
#include "windef.h"

char *ExtractFirstToken(char * lpBuffer, char * szSourceString, char cTokenSeparator);
BYTE PatternSearch(const char * szText, const char * szPattern);

int SplitPath(const char *path, char *drive, char *dir, char *name, char *ext);
void MergePath(char *path, const char *drive, const char *dir, const char *name, const char *ext);
DWORD ConvertWordsToDoubleWord(WORD wHigh, WORD wLow);
char * ConvertFileNameToProperFormat(char * szFile, char * szFileName, char * szExt);
void ToDosFileName(const char* filename, char* fname, unsigned int FNameLength);

