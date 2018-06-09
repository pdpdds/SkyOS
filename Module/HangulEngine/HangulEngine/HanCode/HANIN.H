/*
 *      File HANIN.H
 *      A Header for Hangul Input Library
 *      '90.8.10
 *      '91.1.25, 3.25-28, 4.7-8, 6.19-22, 8.15-18, 9.15, 10.17
 *      '92.1.6,12,18,26, 2.5, 4.25
 *      '93.3.25, 7.13, 8.7
 *      '94.10.10, 12.7,13,26
 *      '95.4.28, 6.8, 8.20,25, 9.2-3,5,9-10,16,18,22, 10.7,9
 *      '95.1.5, 2.7-8
 *      Written by Inkeon Lim
 *
 *      Modified by fontutil ('07.7.28)
 */


#ifndef _HANIN_H_
#define _HANIN_H_

#include "windef.h"
#include "stdint.h"
#include "typedefs.h"

/** Function declarations **************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Hangul code checking */

bool ishangul1st(byte *s, int pos);
bool ishangul2nd(byte *s, int pos);

#ifdef __cplusplus
}
#endif

#endif
