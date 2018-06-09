/*
 *      File C_CNVTHJ.C
 *      Function code_cnvthanja
 *      '95.9.10
 *      Written by Inkeon Lim
 */


#include "hancode.h"


void code_cnvthanja(byte *dest, byte src1, byte src2)
{
    byte c1, c2;
    int hjidx, index;
    codeidx_t codeidx;

    _code->getidx(&codeidx, src1, src2);
    if (codeidx.kind != CODEKIND_HANJA) return;
    hjidx = codeidx.index;

    codeidx.kind = CODEKIND_HANGUL;
    for (index = 0; index <= MAXHANJATBL; index++) {
        codeidx.han = _hanjatbl[index].han;
        _code->getcode(&c1, &c2, &codeidx);
        if (index == MAXHANJATBL || hjidx < _hanjatbl[index + 1].index)
            break;
    }

    dest[0] = c1;
    dest[1] = c2;
}
