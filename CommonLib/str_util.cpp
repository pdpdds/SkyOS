#include "str_util.h"
#include "string.h"
#include "ctype.h"


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


