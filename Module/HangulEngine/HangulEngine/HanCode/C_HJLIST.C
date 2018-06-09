/*
 *      File C_HJLIST.C
 *      Get Hanja List
 *      '95.9.9-10,22
 *      Written by Inkeon Lim
 */

#include "hanin.h"
#include "hancode.h"

void* malloc(size_t size);
void free(void *p);

byte *code_gethanjalist(byte *hangul, int *curcharp)
{
    byte *hanjalist;
    int index;

    if (!ishangul1st(hangul, 0)) return NULL;
    index = code__gethanjaindex(hangul);
    if (index == ERROR) return NULL;

    hanjalist = (byte *)malloc(2 + _hanjatbl[index].n * 2 + 1);
    if (hanjalist == NULL) return NULL;

    hanjalist[0] = hangul[0];
    hanjalist[1] = hangul[1];
    code__gethanjalist(hanjalist + 2, index);

    *curcharp = _hanjatbl[index].curchar;

    return hanjalist;
}

void code_freehanjalist(byte *hanjalist)
{
    if (hanjalist != NULL) 
		free(hanjalist);
}
