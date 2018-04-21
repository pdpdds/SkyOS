#include "wchar.h"

wchar_t* wcschr(const wchar_t* str, wchar_t c)
{
	for (; *str; str++)
		if (*str == c)
			return (wchar_t*) str;

	return NULL;
}