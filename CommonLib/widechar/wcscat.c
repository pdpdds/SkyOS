#include "wchar.h"

wchar_t *wcscat(wchar_t* dest, const wchar_t* src)
{
	const wchar_t* ret = dest;

	while (*dest)
		dest++;

	while (*src)
		*dest++ = *src++;

	*dest = 0;
	return (wchar_t*) ret;
}