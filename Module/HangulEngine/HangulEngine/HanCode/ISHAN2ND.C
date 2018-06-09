/*
 *      File ISHAN2ND.C
 *      Function ishangul2nd
 *      '90.9.2
 *      Written by Inkeon Lim
 *
 *      Modified by fontutil ('07.7.28)
 */


#include <ctype.h>
#include <string.h>

#include "hanin.h"


bool ishangul2nd(byte *s, int pos)
{
    int i;

    if (pos < 1 || pos > (int)strlen(s) - 1 || s[pos - 1] < 128) return false;

    for (i = 0; i < pos; )
        if (isascii(s[i])) i++;
        else i += 2;

    return ((i == pos + 1) ? true : false);
}
