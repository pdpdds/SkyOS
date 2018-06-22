#pragma once
#include "windef.h"
void *bsearch(register const void *key, const void *base0,
	size_t nmemb, register size_t size,
	register int(*compar)(const void *, const void *));