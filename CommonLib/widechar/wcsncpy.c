#include "wchar.h"

wchar_t* wcsncpy(wchar_t* Dst, const wchar_t* Src, unsigned int Count)
{
	wchar_t* RetVal=Dst;

	for(; (*Src != '\0') && (Count != 0); Count--)
		*Dst++=*Src++;
	*Dst='\0';
	return(RetVal);
}
