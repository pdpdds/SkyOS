/*
 *      File C_7BIT.C
 *      Code-to-index Conversion Function Set
 *      for 7 Bit Fixed Hangul Code
 *      '95.4.28-29
 *      Written by Inkeon Lim
 */


//#include <stdlib.h>
#include "windef.h"
#include "extkey.h"
#include "hancode.h"
#include "bsearch.h"

static void code_getidx(codeidx_t *codeidx, byte code1, byte code2);
static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx);
static int code_compare(const void *p1, const void *p2);


code_t _code_7BITFIXED = {code_getidx, code_getcode};


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2)
{
    byte s[2];
    const byte *tblptr;
    byte initial, medial, final;

    codeidx->code1 = code1, codeidx->code2 = code2;

    s[0] = code1, s[1] = code2;
    tblptr = bsearch(s, _cnvtbl_7BITHAN, sizeof(_cnvtbl_7BITHAN) / 2, 2,
                     code_compare);
    if (tblptr == NULL) {
        codeidx->kind = CODEKIND_NOCODE;
        return;
    }

    codeidx->kind = CODEKIND_HANGUL;

    while (tblptr > _cnvtbl_7BITHAN && *(tblptr + 1) == *(tblptr - 1))
        tblptr -= 2;
    tblptr = _cnvtbl_KSHAN + (int)(tblptr - _cnvtbl_7BITHAN);
    code1 = *tblptr++, code2 = *tblptr;

    initial = (code1 & 0x7c) >> 2;
    medial = ((code1 & 0x03) << 3) | (code2 >> 5);
    final = code2 & 0x1f;
    initial = _codetbl_COMMON.code2idx[INITIAL][initial];
    medial = _codetbl_COMMON.code2idx[MEDIAL][medial];
    final = _codetbl_COMMON.code2idx[FINAL][final];

    codeidx->han.initial = H1_FC + initial;
    codeidx->han.medial = H2_FC + medial;
    codeidx->han.final = H3_FC + final;
}

static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx)
{
    const byte (*idx2code)[27 + 1] = _codetbl_COMMON.idx2code;
    const byte *tblptr;
    byte s[2];

    switch (codeidx->kind) {
    case CODEKIND_HANGUL:
        s[0] = 128 | (idx2code[INITIAL][codeidx->han.initial - H1_FC] << 2) |
               (idx2code[MEDIAL][codeidx->han.medial - H2_FC] >> 3);
        s[1] = (idx2code[MEDIAL][codeidx->han.medial - H2_FC] << 5) |
               idx2code[FINAL][codeidx->han.final - H3_FC];

        tblptr = bsearch(s, _cnvtbl_KSHAN, sizeof(_cnvtbl_KSHAN) / 2, 2,
                         code_compare);
        if (tblptr == NULL) *code1 = ' ', *code2 = ' ';
        else {
            tblptr = _cnvtbl_7BITHAN + (int)(tblptr - _cnvtbl_KSHAN);
            *code1 = *tblptr++, *code2 = *tblptr;
        }
        break;
    default:
        *code1 = ' ', *code2 = ' ';
        break;
    }
}

static int code_compare(const void *p1, const void *p2)
{
    const byte *pp1 = (const byte *)p1, *pp2 = (const byte *)p2;

    if (*pp1 != *pp2) return (*pp1 - *pp2);
    else return (*(pp1 + 1) - *(pp2 + 1));
}
