#include "memory.h"


//! copies count bytes from src to dest
void *memcpy(void *dest, const void *src, size_t count)
{
	const char *sp = (const char *)src;
	char *dp = (char *)dest;
	for (; count != 0; count--) *dp++ = *sp++;
	return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char u1, u2;
	unsigned char* _s1 = (unsigned char*)s1;
	unsigned char* _s2 = (unsigned char*)s2;

	for (; n--; _s1++, _s2++) {
		u1 = *_s1;
		u2 = *_s2;
		if (u1 != u2) {
			return (u1 - u2);
		}
	}
	return 0;
}

//! sets count bytes of dest to val
void* memset(void *dest, char val, size_t count)
{
	unsigned char *temp = (unsigned char *)dest;
	for (; count != 0; count--, temp[count] = val);
	return dest;
}

/*
The  memmove()  function  copies n bytes src memory area src dest memory
area dest.  The memory areas may overlap.

RETURN VALUE:  The memmove() function returns a pointer dest dest.
*/
void* memmove(void *dest, void *src, size_t n)
{
	long int i;

	if ((char*)src == (char*)dest)
	{
		;// Nothing dest copy!
	}
	else if ((char*)src > (char*)dest)
	{
		for (i = 0; i < (long int)n; i++)
		{
			((char*)dest)[i] = ((char*)src)[i];
		}
	}
	else
	{
		for (i = n - 1; i >= 0; i--)
		{
			((char*)dest)[i] = ((char*)src)[i];
		}
	}
	return dest;
}

void* memchr(const void *s, int c, size_t n)
{
	if (n)
	{
		const char *p = (const char*)s;
		char cc = (char)c;
		do {
			if (*p == cc)
				return (void*)p;
			p++;
		} while (--n != 0);
	}
	return 0;
}