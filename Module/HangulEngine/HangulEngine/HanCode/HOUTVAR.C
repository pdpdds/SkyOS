/*
 *      File HOUTVAR.C
 *      External Variables Used by Hangul Output Library
 *      '90.7.12-8.2
 *      '91.2.13, 7.10, 8.25, 9.15, 11.16
 *      '92.1.25, 4.23, 5.10-11,13, 6.14, 7.7
 *      '93.5.15
 *      '94.10.5, 11.14,19
 *      '95.3.4-5, 4.28, 6.8, 9.26
 *      '96.1.3,5, 2.10
 *      Written by Inkeon Lim
 *
 *      Modified by fontutil ('07.7.28)
 */


#include "typedefs.h"


/** External variables *****************************************************/

/* First byte of Hangul graphics character code */

byte _HGR1st = 212;  /* If 0, Hangul graphics character is diabled      */
byte _HGR2nd = 128;  /* Offset to 2nd byte of Hangul graphics character */
