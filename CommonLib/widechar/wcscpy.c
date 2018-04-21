#include "wchar.h"

wchar_t *wcscpy(wchar_t *strDestination, const wchar_t *strSource)
{
	wchar_t* dest = strDestination;

	while (*strSource)
		*strDestination++ = *strSource++;

	*strDestination = 0;
	return dest;
}