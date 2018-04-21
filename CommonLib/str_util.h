#pragma once
#include "windef.h"

char *ExtractFirstToken(char * lpBuffer, char * szSourceString, char cTokenSeparator);
BYTE PatternSearch(const char * szText, const char * szPattern);

int SplitPath(const char *path, char *drive, char *dir, char *name, char *ext);
void MergePath(char *path, const char *drive, const char *dir, const char *name, const char *ext);
DWORD ConvertWordsToDoubleWord(WORD wHigh, WORD wLow);
char * ConvertFileNameToProperFormat(char * szFile, char * szFileName, char * szExt);
void ToDosFileName(const char* filename, char* fname, unsigned int FNameLength);

int str_pattern_search(const char * src, const char * pattern);

char* str_rtrim(char * src);
char* str_atrim(char * src);
char* str_ltrim(char * src);
void str_replace(char * src, char oldch, char newch);

char * str_get_token(char * src, unsigned int token_no, char token_separator, char * buf, int buf_size);
char* str_get_token_info(char * src, unsigned int token_no, char token_separator, unsigned int * token_len);
int str_total_tokens(char * src, char ch);
int str_total_characters(char * src, char ch);