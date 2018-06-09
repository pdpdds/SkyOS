/*
 *      File C_ISHJ.C
 *      Function code_ishanja
 *      '95.9.10
 *      Written by Inkeon Lim
 */


#include "hancode.h"


bool code_ishanja(byte *s)
{
    codeidx_t codeidx;

    _code->getidx(&codeidx, s[0], s[1]);

    return (codeidx.kind == CODEKIND_HANJA);
}
