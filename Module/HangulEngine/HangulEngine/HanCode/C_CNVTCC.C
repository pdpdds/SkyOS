/*
 *      File C_CNVTCC.C
 *      Function code_cnvtcc
 *      '95.4.29-30
 *      Written by Inkeon Lim
 */


#include "hancode.h"


void code_cnvtcc(code_t *destcode, code_t *srccode, byte *dest1, byte *dest2,
                 byte src1, byte src2)
{
    codeidx_t codeidx;

    srccode->getidx(&codeidx, src1, src2);
    destcode->getcode(dest1, dest2, &codeidx);
}
