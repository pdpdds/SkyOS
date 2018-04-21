#include "wchar.h"

wchar_t* wcsrchr(const wchar_t* str, int c)
{
	const wchar_t *start = str;
	while (*str)
		str++;

	for (; str >= start; str--)
		if (*str == c)
			return (wchar_t*) str;

	return NULL;
}