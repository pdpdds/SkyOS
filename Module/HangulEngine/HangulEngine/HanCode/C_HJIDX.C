/*
 *      File C_HJIDX.C
 *      Get Hanja Index and List
 *      '95.9.9-10
 *      Written by Inkeon Lim
 */


#include "hancode.h"


int code__gethanjaindex(byte *hangul)
{
    int low = 0, mid, high = MAXHANJATBL;
    byte c1, c2;
    word key, code;
    codeidx_t codeidx;

    key = ((unsigned)hangul[0] << 8) | hangul[1];
    codeidx.kind = CODEKIND_HANGUL;
    while (low <= high) {
        mid = (high + low) / 2;

        codeidx.han = _hanjatbl[mid].han;
        _code->getcode(&c1, &c2, &codeidx);
        code = ((unsigned)c1 << 8) | c2;

        if (code == key) return mid;
        else if (code > key) high = mid - 1;
        else low = mid + 1;
    }

    return ERROR;
}

void code__gethanjalist(byte *hanjalist, int index)
{
    int i, n = _hanjatbl[index].n, index0 = _hanjatbl[index].index;
    codeidx_t codeidx;

    codeidx.kind = CODEKIND_HANJA;
    for (i = 0; i < n; i++) {
        codeidx.index = index0 + i;
        _code->getcode(&hanjalist[i * 2], &hanjalist[i * 2 + 1], &codeidx);
    }
    hanjalist[i * 2] = '\0';
}
