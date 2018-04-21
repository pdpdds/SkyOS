#include "wchar.h"

int wcscmp(const wchar_t *str1, const wchar_t *str2)
{
	while((*str2 != L'\0') && (*str1 == *str2))
	{
		str1++;
		str2++;
	}
	return *str1 -  *str2;
}
