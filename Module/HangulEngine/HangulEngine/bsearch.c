#include "bsearch.h"
#include "windef.h"

void *bsearch(register const void *key, const void *base0,
	size_t nmemb, register size_t size,
	register int(*compar)(const void *, const void *))
{
	register const char *base = (const char *)base0;
	register int lim, cmp;
	register const void *p;

	for (lim = nmemb; lim != 0; lim >>= 1) {
		p = base + (lim >> 1) * size;
		cmp = (*compar)(key, p);
		if (cmp == 0)
			return (void *)p;
		if (cmp > 0) {	/* key > p: move right */
			base = (const char *)p + size;
			lim--;
		} /* else move left */
	}
	return (NULL);
}
