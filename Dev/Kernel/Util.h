#pragma once
#include "windef.h"
#include "SysInfo.h"
#ifdef _DEBUG_ 
    #define DEBUG_PRINT_INFO(Message) dbgprintf("\n%s:%d:%s(): %s", __FILE__ , __LINE__,__PRETTY_FUNCTION__, Message );
    #define DEBUG_PRINT_OBJECT1(Format, Object) \
        {\
            dbgprintf("\n%s:%d:%s() ", __FILE__ , __LINE__,__PRETTY_FUNCTION__);\
            dbgprintf(Format, Object );\
        }
    #define DEBUG_PRINT_OBJECT2(Format, Object1, Object2) \
        {\
            dbgprintf("\n%s:%d:%s() ", __FILE__ , __LINE__,__PRETTY_FUNCTION__);\
            dbgprintf(Format, Object1, Object2 );\
        }
    #define DEBUG_PRINT_OBJECT3(Format, Object1, Object2, Object3) \
        {\
            dbgprintf("\n%s:%d:%s() ", __FILE__ , __LINE__,__PRETTY_FUNCTION__);\
            dbgprintf(Format, Object1, Object2, Object3);\
        }
#else
    #define DEBUG_PRINT_INFO(Message) 
    #define DEBUG_PRINT_OBJECT1(Format, Object1) 
    #define DEBUG_PRINT_OBJECT2(Format, Object1, Object2) 
    #define DEBUG_PRINT_OBJECT3(Format, Object1, Object2, Object3)
    
#endif

#define ALLOC_AND_CHECK(To, Cast, Size, ReturnValueOnFail) \
    To = (Cast)malloc(Size);\
    if ( To == NULL )\
    {\
        DEBUG_PRINT_OBJECT1("Memory allocation failed while allocating %ld bytes", Size );\
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);\
        return ReturnValueOnFail;\
    }


char * ExtractFirstToken(char * lpBuffer, char * szSourceString, char cTokenSeparator);
//static __inline__ char * MaxPtr(char * p1, char * p2);
BYTE PatternSearch(const char * szText, const char * szPattern);

int SplitPath(const char *path, char *drive, char *dir, char *name, char *ext);
void MergePath(char *path, const char *drive, const char *dir, const char *name, const char *ext);

static  char* MaxPtr(char * p1, char * p2)
{
    if (p1 > p2)
        return p1;
    else
        return p2;
}

static  DWORD ConvertWordsToDoubleWord(WORD wHigh, WORD wLow)
{
    DWORD dwResult;
    dwResult = wHigh;
    dwResult <<= 16;
    dwResult |= wLow;
    return dwResult;
}

