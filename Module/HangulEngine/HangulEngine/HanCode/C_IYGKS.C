/*
 *      File C_IYGKS.C
 *      Code-to-index Conversion Function Set
 *      for Iyagi Extended KS Fixed Code
 *      '95.4.29
 *      Written by Inkeon Lim
 */


#include "windef.h"
#include "extkey.h"
#include "hancode.h"
#include "bsearch.h"

static void code_getidx(codeidx_t *codeidx, byte code1, byte code2);
static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx);
static int code_compare(const void *p1, const void *p2);


code_t _code_IYGKS = {code_getidx, code_getcode};


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2)
{
    const byte *tblptr;
    byte initial, medial, final;

    codeidx->code1 = code1, codeidx->code2 = code2;

    if (code2 < 0xa1 || code2 > 0xfe)
        codeidx->kind = CODEKIND_NOCODE;
    else if (code1 >= 0xb0 && code1 <= 0xc8) {
        codeidx->kind = CODEKIND_HANGUL;

        tblptr = _cnvtbl_KSHAN + ((code1 - 0xb0) * 94 + (code2 - 0xa1)) * 2;
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
    } else if (code1 >= 0xca && code1 <= 0xfd) {
        codeidx->kind = CODEKIND_HANJA;
        codeidx->index = (code1 - 0xca) * 94 + (code2 - 0xa1);
    } else if (code1 >= 0xa1 && code1 <= 0xac) {
        codeidx->kind = CODEKIND_SPECIAL;
        codeidx->index = (code1 - 0xa1) * 94 + (code2 - 0xa1);
    } else if (code1 >= 0xad && code1 <= 0xae) {
        codeidx->kind = CODEKIND_TGSPECIAL;
        codeidx->index = (code1 - 0xad) * 94 + (code2 - 0xa1);
        if (codeidx->index >= 128) codeidx->kind = CODEKIND_NOCODE;
    } else codeidx->kind = CODEKIND_NOCODE;
}

static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx)
{
    const byte (*idx2code)[27 + 1] = _codetbl_COMMON.idx2code;
    const byte *tblptr;
    byte s[2];
    int index;

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
            index = (int)(tblptr - _cnvtbl_KSHAN) / 2;
            *code1 = (index / 94) + 0xb0;
            *code2 = (index % 94) + 0xa1;
        }
        break;
    case CODEKIND_HANJA:
        *code1 = (codeidx->index / 94) + 0xca;
        *code2 = (codeidx->index % 94) + 0xa1;
        break;
    case CODEKIND_SPECIAL:
        *code1 = (codeidx->index / 94) + 0xa1;
        *code2 = (codeidx->index % 94) + 0xa1;
        break;
    case CODEKIND_TGSPECIAL:
        *code1 = (codeidx->index / 94) + 0xad;
        *code2 = (codeidx->index % 94) + 0xa1;
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
