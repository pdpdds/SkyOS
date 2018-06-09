/*
 *      File C_SETHJ.C
 *      Function code_setcurhanja
 *      '95.9.10
 *      Written by Inkeon Lim
 */


#include "hancode.h"


void code_setcurhanja(byte *hangul, int curchar)
{
    int index;

    index = code__gethanjaindex(hangul);
    if (index != ERROR) _hanjatbl[index].curchar = curchar;
}
