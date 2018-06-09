/*
 *      File HANTYPES.H
 *      Types Used in Hangul Lib
 *      '92.5.10,13,22-24
 *      '93.1.5, 7.20
 *      '95.4.29, 6.5-7
 *      '96.1.8
 *      Written by Inkeon Lim
 *
 *      Modified by fontutil ('07.7.28)
 */


#ifndef _HANTYPES_H_
#define _HANTYPES_H_

#ifndef _TYPEDEFS_H_
    #include "typedefs.h"
#endif


/** Type definitions *******************************************************/

/* The kind of codes */

typedef enum {
    CODEKIND_NOCODE, CODEKIND_HANGUL, CODEKIND_HANJA,
    CODEKIND_SPECIAL, CODEKIND_TGSPECIAL,
} codekind_t;

/* Hangul index */

typedef struct {
    byte initial;
    byte medial;
    byte final;
} han_t;

/* Code index */

typedef struct {
    codekind_t kind;
    byte code1, code2;  /* Raw code            */
    han_t han;          /* CODEKIND_HANGUL     */
    int index;          /* Other CODEKIND_...s */
} codeidx_t;

#endif
