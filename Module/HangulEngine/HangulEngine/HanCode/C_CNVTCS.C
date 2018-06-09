/*
 *      File C_CNVTCS.C
 *      Function code_cnvtcs
 *      '95.4.29-30
 *      Written by Inkeon Lim
 */


#include "hancode.h"


void code_cnvtcs(code_t *destcode, code_t *srccode, byte *dest, byte src1,
                 byte src2)
{
    codeidx_t codeidx;

    srccode->getidx(&codeidx, src1, src2);
    destcode->getcode(&dest[0], &dest[1], &codeidx);
}
