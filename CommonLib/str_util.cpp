#include "str_util.h"
#include "string.h"
#include "ctype.h"
#include "memory.h"


char* MaxPtr(char * p1, char * p2)
{
	if (p1 > p2)
		return p1;
	else
		return p2;
}

DWORD ConvertWordsToDoubleWord(WORD wHigh, WORD wLow)
{
	DWORD dwResult;
	dwResult = wHigh;
	dwResult <<= 16;
	dwResult |= wLow;
	return dwResult;
}

char * ExtractFirstToken(char * lpBuffer, char * szSourceString, char cTokenSeparator)
{
    char * szToken = 0;
    int i;
    
    lpBuffer[0] = 0;
    for(i=0; szSourceString[i]; i++)
		if ( szSourceString[i] == cTokenSeparator )	
		{
			szToken = &szSourceString[i];
			break;
		}
    if ( !szToken )
        return 0;
    
	strncpy( lpBuffer, szSourceString, szToken-szSourceString);
    lpBuffer[ szToken - szSourceString ]=0;

    return szToken;
}
BYTE PatternSearch(const char * szText, const char * szPattern)
{
    int  i, slraw;
    #ifdef _DEBUG_
        dbgprintf("\n UTIL :: PattenSearch [%s], [%s]", szText, szPattern);
    #endif

    if ((*szPattern == '\0') && (*szText == '\0'))    /*  if it is end of both  */
        return 1;                                     /*  strings,then match    */
    if (*szPattern == '\0')                           /*  if it is end of only  */
        return 0;                                 /*  szPattern tehn mismatch     */
    if (*szPattern == '*')                            /*  if pattern is a '*'    */
    {
        if (*(szPattern+1) == '\0')                   /*  if it is end of szPattern */
            return 1;                             /*    then match          */
        for(i=0,slraw=strlen(szText);i<=slraw;i++)    /*    else hunt for match*/
            if ((*(szText+i) == *(szPattern+1)) ||    /*         or wild card   */
                                (*(szPattern+1) == '?'))
                if (PatternSearch(szText+i+1,szPattern+2) == 1)    /*      if found,match    */
                    return( 1 ) ;                       /*        rest of szPattern     */
    }
    else
    {
        if ( *szText == '\0' )                          /*  if end of szText then    */
            return 0;                                   /*     mismatch           */
        if ( (*szPattern == '?') || (*szPattern == *szText) )  /*  if chars match then   */
            if ( PatternSearch(szText+1,szPattern+1) == 1)     /*  try & match rest of it*/
               return 1;
    }
    #ifdef _DEBUG_
        dbgprintf("\n UTIL :: PattenSearch No match");
    #endif
    return 0;                                                  /*  no match found        */
}

int SplitPath(const char *path, char *drive, char *dir, char *name, char *ext)
{
    int flags = 0, len;
    const char *pp, *pe;
    if (drive)
        *drive = '\0';
    if (dir)
        *dir = '\0';
    if (name)
        *name = '\0';
    if (ext)
        *ext = '\0';

    pp = path;
    if ((isalpha(*pp) || strchr("[\\]^_`", *pp)) && (pp[1] == ':'))
    {
        if (drive)
        {
            strncpy(drive, pp, 2);
            drive[2] = '\0';
        }
        pp += 2;
    }
    pe = MaxPtr( (char *) strrchr(pp, '\\'),(char *) strrchr(pp, '/'));
    if (pe) 
    {
        pe++;
        len = pe - pp;
        if (dir)
        {
            strncpy(dir, pp, len);
            dir[len] = '\0';
        }
        pp = pe;
    }
    else
        pe = pp;
    if ( *pp == NULL ) //sam
        return 0;
    /* Special case: "c:/path/." or "c:/path/.."
    These mean FILENAME, not EXTENSION.  */
    while (*pp == '.')
        ++pp;
    if (pp > pe)
    {
        if (name)
        {
            len = pp - pe;
            strncpy(name, pe, len);
            name[len] = '\0';
          /* advance name over '.'s so they don't get scragged later on when the
           * rest of the name (if any) is copied (for files like .emacs). - WJC
           */
            name+=len;
        }
    }

    pe = strrchr(pp, '.');
    if (pe)
    {
        if (ext) 
            strcpy(ext, pe);
    }
    else 
        pe =  pp + strlen(pp);

    if (pp != pe)
    {
        
        if ( pe > pp )
            len = pe - pp;
        else
            len = 0;
        if (name)
        {
            strncpy(name, pp, len);
            name[len] = '\0';
        }
    }
    return flags;
}
void MergePath(char *path, const char *drive, const char *dir, const char *name, const char *ext)
{
    *path = '\0';
    if (drive && *drive)
    {
        path[0] = drive[0];
        path[1] = ':';
        path[2] = 0;
    }
    if (dir && *dir)
    {
        char last_dir_char = dir[strlen(dir) - 1];
        strcat(path, dir);
        if (last_dir_char != '/' && last_dir_char != '\\')
                strcat(path, strchr(dir, '\\') ? "\\" : "/");
    }
    if (name)
        strcat(path, name);
    if (ext && *ext)
    {
        if (*ext != '.')
            strcat(path, ".");
        strcat(path, ext);
    }
}

char * ConvertFileNameToProperFormat(char * szFile, char * szFileName, char * szExt)
{
	int i;
	szFile[0] = 0;
	if (szFileName[0])
	{
		strcpy(szFile, szFileName);
		for (i = strlen(szFile); i<8; i++)
			szFile[i] = ' ';
		szFile[8] = 0;
	}
	if (szExt[0])
	{
		strcat(szFile, &szExt[1]);
		for (i = strlen(szFile); i<11; i++)
			szFile[i] = ' ';
		szFile[11] = 0;
	}
	return szFile;
}

/**
*	Helper function. Converts filename to DOS 8.3 file format
*/
void ToDosFileName(const char* filename, char* fname, unsigned int FNameLength) 
{

	unsigned int  i = 0;

	if (FNameLength > 11)
		return;

	if (!fname || !filename)
		return;

	//! set all characters in output name to spaces
	memset(fname, ' ', FNameLength);

	//! 8.3 filename
	for (i = 0; i < strlen(filename) - 1 && i < FNameLength; i++) {

		if (filename[i] == '.' || i == 8)
			break;

		//! capitalize character and copy it over (we dont handle LFN format)
		fname[i] = toupper(filename[i]);
	}

	//! add extension if needed
	if (filename[i] == '.') {

		//! note: cant just copy over-extension might not be 3 chars
		for (int k = 0; k<3; k++) {

			++i;
			if (filename[i])
				fname[8 + k] = filename[i];
		}
	}

	//! extension must be uppercase (we dont handle LFNs)
	for (i = 0; i < 3; i++)
		fname[8 + i] = toupper(fname[8 + i]);
}


int str_pattern_search(const char * src, const char * pattern)
{
	int  i, slraw;

	/*  if it is end of both then strings match */

	if ((*pattern == '\0') && (*src == '\0'))
		return 1;

	/*  if it is end of only pattern then mismatch*/
	if (*pattern == '\0')
		return 0;

	/*  if pattern is a '*'    */
	if (*pattern == '*')
	{
		/*  if it is end of pattern then match */
		if (*(pattern + 1) == '\0')
			return 1;

		/*    else hunt for match or wild card   */
		for (i = 0, slraw = strlen(src); i <= slraw; i++)
			if ((*(src + i) == *(pattern + 1)) || (*(pattern + 1) == '?'))
				if (str_pattern_search(src + i + 1, pattern + 2) == 1)    /*  if found, match rest of pattern   */
					return 1;
	}
	else
	{
		/*  if end of src then mismatch   */
		if (*src == '\0')
			return 0;

		/*  if chars match then try & match rest of it*/
		if ((*pattern == '?') || (*pattern == *src))
			if (str_pattern_search(src + 1, pattern + 1) == 1)
				return 1;
	}

	/*no match found*/
	return 0;
}

int str_total_characters(char * src, char ch)
{
	int i = 0;

	while (*src)
	{
		if (*src == ch)
			i++;

		src++;
	}

	return i;
}

int str_total_tokens(char * src, char ch)
{
	int i, result = 0;
	char prev = 0;

	for (i = 0; src[i]; i++)
	{
		if (src[i] == ch && prev != src[i])
			result++;

		prev = src[i];
	}

	/*if the string not ends with separator then we have one more token*/
	if (prev != ch)
		result++;

	return result;
}

char* str_get_token_info(char * src, unsigned int token_no, char token_separator, unsigned int * token_len)
{
	unsigned int i = 0;

	/*find the start position of the token*/

	while (i < token_no)
	{
		char * start = strchr(src, token_separator);
		/*skip consecutive separators*/

		while (start && *start == token_separator)
			start++;

		if (start == NULL)
			return NULL;

		src = start;

		i++;
	}

	/*find and update the token size*/
	for (i = 0; src[i] && src[i] != token_separator; i++)
		;

	*token_len = i;

	return src;
}

char * str_get_token(char * src, unsigned int token_no, char token_separator, char * buf, int buf_size)
{
	int unsigned token_len;
	char *token = str_get_token_info(src, token_no, token_separator, &token_len);
	if (buf_size < (int)token_len)
		token_len = buf_size;

	if (token)
	{
		strncpy(buf, token, token_len);
		buf[token_len] = 0;
		return buf;
	}
	else
	{
		buf[0] = 0;
		return NULL;
	}
}

void str_replace(char * src, char oldch, char newch)
{
	int i;
	//cant replace null character

	if (oldch == 0)
		return;

	for (i = 0; src[i]; i++)
		if (src[i] == oldch)
			src[i] = newch;
}

char* str_ltrim(char * src)
{
	int i, j;

	if (src[0] == 0)
		return NULL;

	for (i = 0; src[i] == ' '; i++)
		;

	if (i != 0)
	{
		for (j = 0; src[i]; i++, j++)
			src[j] = src[i];

		src[j] = 0;
	}

	return src;
}

/*!Remove the space characters in the right from a string
*/
char* str_rtrim(char * src)
{
	int i;

	if (src[0] == 0)
		return NULL;

	for (i = strlen(src) - 1; src[i] == ' '; i--)
		;

	src[i + 1] = 0;

	return src;
}

/*!Remove the space characters in the right and left from a string
*/
char* str_atrim(char * src)
{
	return str_rtrim(str_ltrim(src));
}