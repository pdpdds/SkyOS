#include "wchar.h"

size_t wcslen(const wchar_t *str)
{
	size_t count;

	for (count = 0; *str; str++, count++)
		;

	return count;
}