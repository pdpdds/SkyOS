/*
 *      File C_CNVTS.C
 *      Function code_cnvts
 *      '95.4.30
 *      Written by Inkeon Lim
 */


#include "hancode.h"


void code_cnvts(code_t *destcode, code_t *srccode, byte *dest, byte *src)
{
    codeidx_t codeidx;

    while (*src)
        if (*src >= 128) {
            srccode->getidx(&codeidx, src[0], src[1]);
            destcode->getcode(&dest[0], &dest[1], &codeidx);
            src += 2, dest += 2;
        } else *dest++ = *src++;

    *dest = '\0';
}
