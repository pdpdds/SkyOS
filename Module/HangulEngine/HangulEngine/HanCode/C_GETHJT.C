/*
 *      File C_GETHJT.C
 *      Function code_gethanjatbl, code_sethanjatbl
 *      '95.9.16
 *      Written by Inkeon Lim
 */


#include "hancode.h"


void code_gethanjatbl(int curchars[])
{
    int i;

    for (i = 0; i < N_HANJATBL; i++)
        curchars[i] = _hanjatbl[i].curchar;
}

int code_sethanjatbl(int curchars[])
{
    int i, errcode = NOERROR;

    for (i = 0; i < N_HANJATBL; i++)
        if (curchars[i] < _hanjatbl[i].n)
            _hanjatbl[i].curchar = curchars[i];
        else errcode = ERROR;

    return errcode;
}
