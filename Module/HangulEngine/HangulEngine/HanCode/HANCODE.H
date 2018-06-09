/*
 *      File HANCODE.H
 *      A Header for Hangul Code
 *      '92.5.24
 *      '95.4.27-30, 5.2, 9.8,10,16
 *      Written by Inkeon Lim
 *
 *		Modified by fontutil ('07.7.28)
 */


#ifndef _HANCODE_H_
#define _HANCODE_H_

#include "windef.h"
#ifndef _HANTYPES_H_
    #include "hantypes.h"
#endif


/** Macro constants ********************************************************/

#define FC 0    /* Fill Code */
#define NC 255  /* No Code   */

#ifndef INITIAL
    #define INITIAL 0
    #define MEDIAL  1
    #define FINAL   2
#endif

/*
 *  KSSM(Korean Standard Specification Model) code
 *  including TG code(== HP, THP, CHP and NKP code)
 *  consist of all Hangul, 4888 Hanja, 1128 KS special
 *  and 128 TG special characters
 */
#define CODE_KSSM (&_code_KSSM)

/*
 *  KS C 5601-1987(Korean industrial Standard) fixed code(1987.3.7)
 *  consist of 2350 Hangul, 4888 Hanja and 1128 KS special characters
 */
#define CODE_KS (&_code_KS)

/*
 *  CP949 Extended KS fixed code
 *  consist of all Hangul, 4888 Hanja and 1128 KS special characters
 */
#define CODE_CP949 (&_code_CP949)

/*
 *  Iyagi extended KS fixed code
 *  consist of Hangul, Hanja, KS special and TG special characters
 *  This code includes all of CODE_KS.
 */
#define CODE_IYGKS (&_code_IYGKS)

/*
 *  All kinds of combined Hangul codes
 *  consist of ONLY Hangul characters
 */
#define CODE_COMMON   (&_code_COMMON)    /* Common/Sambo combined code */
#define CODE_SAMSUNG  (&_code_SAMSUNG)   /* Samsung combined code      */
#define CODE_GOLDSTAR (&_code_GOLDSTAR)  /* GoldStar combined code     */
                                         /* (KS C 5601-1982)           */
#define CODE_OLDKS    (&_code_OLDKS)     /* Old KS combined code       */
#define CODE_DKBII    (&_code_DKBII)     /* Dkby II combined code      */

/*
 *  7 bit fixed Hangul code
 *  consist of ONLY Hangul characters
 */
#define CODE_7BITFIXED (&_code_7BITFIXED)

/* The maximum value of the index of the Hangul-Hanja conversion table */

#define MAXHANJATBL 472
#define N_HANJATBL (MAXHANJATBL + 1)

/* The maximum value of the number of Hanjas per Hangul */

#define MAXNHANJA 64


/** Type definitions *******************************************************/

/* Code-to-index conversion function set */

typedef struct {
    void (*getidx)(codeidx_t *codeidx, byte code1, byte code2);
    void (*getcode)(byte *code1, byte *code2, codeidx_t *codeidx);
} code_t;

/* Code table for combined Hangul codes */

typedef struct {
    byte code2idx[3][32];
    byte idx2code[3][27 + 1];
} codetbl_t;

/* Hangul-Hanja conversion table */

typedef struct {
    han_t han;
    uchar n;
    int index;
    uchar curchar;
} hanjatbl_t;


/** Function declarations **************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Convert a character to a character/string */

void code_cnvtcc(code_t *destcode, code_t *srccode, byte *dest1, byte *dest2,
                 byte src1, byte src2);
void code_cnvtcs(code_t *destcode, code_t *srccode, byte *dest, byte src1,
                 byte src2);
void code_cnvthanja(byte *dest, byte src1, byte src2);

/* Convert a string */

void code_cnvts(code_t *destcode, code_t *srccode, byte *dest, byte *src);

/* Conversion function set for all combined Hangul codes */

void code__getidx(codetbl_t *codetbl, codeidx_t *codeidx, byte code1,
                  byte code2);
void code__getcode(codetbl_t *codetbl, byte *code1, byte *code2,
                   codeidx_t *codeidx);

/* Manipulate the hanja table */

byte *code_gethanjalist(byte *hangul, int *curcharp);
void code_freehanjalist(byte *hanjalist);

int code__gethanjaindex(byte *hangul);
void code__gethanjalist(byte *hanjslist, int index);

void code_setcurhanja(byte *hangul, int curchar);

void code_gethanjatbl(int curchars[]);
int code_sethanjatbl(int curchars[]);

/* Check a code */

//bool code_ishangul(byte *s);
//bool code_ishanja(byte *s);
//bool code_isspecial(byte *s);




/** Macro functions ********************************************************/

/* Set/get the current code */

#define code_set(code) (_code = (code))
#define code_get() _code

/* Set the default code                           */
/* Must be located at the outer area of functions */
/* where external variables are located           */
/* and after #include "hancode.h"                 */

#define DEFCODE(code) code_t *_code = (code);


/** External variables *****************************************************/

/* Code-to-index conversion function sets */

extern code_t _code_KSSM;
extern code_t _code_KS;
extern code_t _code_CP949;
extern code_t _code_IYGKS;
extern code_t _code_COMMON;
extern code_t _code_SAMSUNG;
extern code_t _code_GOLDSTAR;
extern code_t _code_OLDKS;
extern code_t _code_DKBII;
extern code_t _code_7BITFIXED;

/* A pointer to the current code-to-index conversion function set */

extern code_t *_code;

/* A table to convert the KS fixed Hangul code */
/* to the common combined Hangul code          */

extern const byte _cnvtbl_KSHAN[2350 * 2];

/* A table to convert the CP949 Extended KS fixed Hangul code */
/* to the common combined Hangul code          */

extern const word _cnvtbl_CP949HAN[72][190];
extern const word _cnvtbl_COMMON[76][189];

/* A index table to convert the common combined Hangul code */

extern codetbl_t _codetbl_COMMON;

/* A table to convert the index of the KS fixed Hangul code */
/* to the 7 bit fixed Hangul code                           */

extern const byte _cnvtbl_7BITHAN[2350 * 2];

/* Hangul-Hanja conversion table */

extern hanjatbl_t _hanjatbl[MAXHANJATBL + 1];

/* The first byte of Hangul graphics character codes in KSSM */
/* The default is 0xd4(212)                                  */

extern byte _HGR1st;

/* The offset to the 2nd byte of Hangul graphics characters in KSSM */
/* The default is 0x80(128)                                         */

extern byte _HGR2nd;

/* A hook vector to hook No Codes */

extern int (*_hooknocode)(byte code1, byte code2,
                          void *fontbuf, int width, int height);
#ifdef __cplusplus
}
#endif
#endif
