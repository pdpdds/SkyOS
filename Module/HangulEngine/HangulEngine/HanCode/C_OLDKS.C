/*
 *      File C_OLDKS.C
 *      Code-to-index Conversion Function Set
 *      for Old KS C 5601 Combined Hangul Code
 *      '95.4.27-28,30
 *      Written by Inkeon Lim
 */


#include "hancode.h"


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2);
static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx);


code_t _code_OLDKS = {code_getidx, code_getcode};

static codetbl_t _codetbl_OLDKS = {
    {
        {
            NC, NC, NC, NC, NC, NC, NC, NC,  /* Initial */
            NC, FC,  1,  2,  3,  4,  5,  6,
             7,  8,  9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, NC, NC, NC,
        }, {
            NC, FC,  1,  2, NC,  3,  4,  5,  /* Medial */
            NC,  6,  7,  8, NC,  9, 10, 11,
            NC, 12, 13, 14, NC, 15, 16, 17,
            NC, 18, 19, 20, NC, 21, NC, NC,
        }, {
            NC, FC,  1,  2,  3,  4,  5,  6,  /* Final */
             7,  8,  9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22,
            23, 24, 25, 26, 27, NC, NC, NC,
        }
    }, {
        {
             9, 10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24,
            25, 26, 27, 28,
        },  {
             1,  2,  3,  5,  6,  7,  9, 10,
            11, 13, 14, 15, 17, 18, 19, 21,
            22, 23, 25, 26, 27, 29,
        },  {
             1,  2,  3,  4,  5,  6,  7,  8,
             9, 10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24,
            25, 26, 27, 28,
        },
    }
};


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2)
{
    code__getidx(&_codetbl_OLDKS, codeidx, code1, code2);
}

static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx)
{
    code__getcode(&_codetbl_OLDKS, code1, code2, codeidx);
}
