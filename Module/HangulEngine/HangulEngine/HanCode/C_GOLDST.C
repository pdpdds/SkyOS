/*
 *      File C_GOLDSTAR.C
 *      Code-to-index Conversion Function Set
 *      for GoldStar Combined Hangul Code(Mighty-16 Series)
 *      and KS C 5601-1982 Combined Hangul Code
 *      '95.4.27-28,30
 *      Written by Inkeon Lim
 */


#include "hancode.h"


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2);
static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx);


code_t _code_GOLDSTAR = {code_getidx, code_getcode};

static codetbl_t _codetbl_GOLDSTAR = {
    {
        {
            FC,  1,  2, NC,  3, NC, NC,  4,  /* Initial */
             5,  6, NC, NC, NC, NC, NC, NC,
            NC,  7,  8,  9, NC, 10, 11, 12,
            13, 14, 15, 16, 17, 18, 19, NC,
        }, {
            NC, NC,  1,  2,  3,  4,  5,  6,  /* Medial */
            NC, NC,  7,  8,  9, 10, 11, 12,
            NC, NC, 13, 14, 15, 16, 17, 18,
            NC, NC, 19, 20, 21, NC, NC, FC,
        }, {
            FC,  1,  2,  3,  4,  5,  6,  7,  /* Final */
            NC,  8,  9, 10, 11, 12, 13, 14,
            15, 16, 17, NC, 18, 19, 20, 21,
            22, NC, 23, 24, 25, 26, 27, NC,
        }
    }, {
        {
             0,  1,  2,  4,  7,  8,  9, 17,
            18, 19, 21, 22, 23, 24, 25, 26,
            27, 28, 29, 30,
        },  {
            31,  2,  3,  4,  5,  6,  7, 10,
            11, 12, 13, 14, 15, 18, 19, 20,
            21, 22, 23, 26, 27, 28,
        },  {
             0,  1,  2,  3,  4,  5,  6,  7,
             9, 10, 11, 12, 13, 14, 15, 16,
            17, 18, 20, 21, 22, 23, 24, 26,
            27, 28, 29, 30,
        },
    }
};


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2)
{
    code__getidx(&_codetbl_GOLDSTAR, codeidx, code1, code2);
}

static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx)
{
    code__getcode(&_codetbl_GOLDSTAR, code1, code2, codeidx);
}
