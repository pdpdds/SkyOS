/*
 *      File C_DKBII.C
 *      Code-to-index Conversion Function Set
 *      for Dkby II Combined Hangul Code
 *      '95.4.27-28,30
 *      Written by Inkeon Lim
 */


#include "hancode.h"


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2);
static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx);


code_t _code_DKBII = {code_getidx, code_getcode};

static codetbl_t _codetbl_DKBII = {
    {
        {
            FC,  1,  2,  3,  4,  5,  6,  7,  /* Initial */
             8,  9, 10, 11, NC, NC, NC, NC,
            NC, NC, NC, NC, NC, NC, NC, 12,
            13, 14, 15, 16, 17, 18, 19, NC
        }, {
            NC, NC, FC,  1,  2,  3,  4,  5,  /* Medial */
            NC, NC,  6,  7,  8,  9, 10, 11,
            NC, NC, 12, 13, 14, 15, 16, 17,
            NC, NC, 18, 19, 20, 21, NC, NC
        }, {
            FC,  1,  2,  3,  4,  5,  6,  7,  /* Final */
             8,  9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23,
            24, 25, 26, 27, NC, NC, NC, NC
        }
    }, {
        {
             0,  1,  2,  3,  4,  5,  6,  7,
             8,  9, 10, 11, 23, 24, 25, 26,
            27, 28, 29, 30,
        },  {
             2,  3,  4,  5,  6,  7, 10, 11,
            12, 13, 14, 15, 18, 19, 20, 21,
            22, 23, 26, 27, 28, 29,
        },  {
             0,  1,  2,  3,  4,  5,  6,  7,
             8,  9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23,
            24, 25, 26, 27,
        },
    }
};


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2)
{
    code__getidx(&_codetbl_DKBII, codeidx, code1, code2);
}

static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx)
{
    code__getcode(&_codetbl_DKBII, code1, code2, codeidx);
}
