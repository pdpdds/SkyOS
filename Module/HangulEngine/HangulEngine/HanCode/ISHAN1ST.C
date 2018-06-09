/*
 *      File ISHAN1ST.C
 *      Function ishangul1st
 *      '90.9.2
 *      Written by Inkeon Lim
 *
 *      Modified by fontutil ('07.7.28)
 */


#include <ctype.h>
#include <string.h>

#include "hanin.h"


bool ishangul1st(byte *s, int pos)
{
    int i;

    if (pos < 0 || pos > (int)strlen(s) - 2 || s[pos] < 128) return false;

    for (i = 0; i < pos; )
        if (isascii(s[i])) i++;
        else i += 2;

    return ((i == pos) ? true : false);
}
