/*
 *      File C_KSSM.C
 *      Code-to-index Conversion Function Set
 *      for KSSM(Korean Standard Specification Model) Code
 *      '95.4.27-29
 *      Written by Inkeon Lim
 */


#include "extkey.h"
#include "hancode.h"


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2);
static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx);


code_t _code_KSSM = {code_getidx, code_getcode};


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2)
{
    byte initial, medial, final;
    int code2i;

    codeidx->code1 = code1, codeidx->code2 = code2;

    if (code1 == _HGR1st) {
        codeidx->kind = CODEKIND_TGSPECIAL;

        if (code2 < _HGR2nd) codeidx->kind = CODEKIND_NOCODE;

        codeidx->index = code2 - _HGR2nd;
    } else if (code1 >= 0xd9 && code1 <= 0xde) {
        codeidx->kind = CODEKIND_SPECIAL;

        code2i = code2;
        if ((code2i -= 0x31) < 0) codeidx->kind = CODEKIND_NOCODE;
        else if (code2i <= 0x4d) ;
        else if ((code2i -= 0x12) < 0) codeidx->kind = CODEKIND_NOCODE;
        else if (code2i <= 0xbb) ;
        else codeidx->kind = CODEKIND_NOCODE;

        codeidx->index = (code1 - 0xd9) * 94 * 2 + code2i;
    } else if (code1 >= 0xe0 && code1 <= 0xf9) {
        codeidx->kind = CODEKIND_HANJA;

        code2i = code2;
        if ((code2i -= 0x31) < 0) codeidx->kind = CODEKIND_NOCODE;
        else if (code2i <= 0x4d) ;  /* 0x31 ~ 0x7e */
        else if ((code2i -= 0x12) < 0) codeidx->kind = CODEKIND_NOCODE;
        else if (code2i <= 0xbb) ;  /* 0x91 ~ 0xfe */
        else codeidx->kind = CODEKIND_NOCODE;

        codeidx->index = (code1 - 0xe0) * 94 * 2 + code2i;
    } else {
        codeidx->kind = CODEKIND_HANGUL;

        initial = (code1 & 0x7c) >> 2;
        medial = ((code1 & 0x03) << 3) | (code2 >> 5);
        final = code2 & 0x1f;
        initial = _codetbl_COMMON.code2idx[INITIAL][initial];
        medial = _codetbl_COMMON.code2idx[MEDIAL][medial];
        final = _codetbl_COMMON.code2idx[FINAL][final];

        if (initial == NC || medial == NC || final == NC)
            codeidx->kind = CODEKIND_NOCODE;

        codeidx->han.initial = H1_FC + initial;
        codeidx->han.medial = H2_FC + medial;
        codeidx->han.final = H3_FC + final;
    }
}

static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx)
{
    const byte (*idx2code)[27 + 1] = _codetbl_COMMON.idx2code;

    switch (codeidx->kind) {
    case CODEKIND_HANGUL:
        *code1 = 128 |
                 (idx2code[INITIAL][codeidx->han.initial - H1_FC] << 2) |
                 (idx2code[MEDIAL][codeidx->han.medial - H2_FC] >> 3);
        *code2 = (idx2code[MEDIAL][codeidx->han.medial - H2_FC] << 5) |
                 idx2code[FINAL][codeidx->han.final - H3_FC];
        break;
    case CODEKIND_HANJA:
        *code1 = codeidx->index / (94 * 2) + 0xe0;
        *code2 = codeidx->index % (94 * 2) + 0x31;
        if (*code2 > 0x7e) *code2 += 0x12;
        break;
    case CODEKIND_SPECIAL:
        *code1 = codeidx->index / (94 * 2) + 0xd9;
        *code2 = codeidx->index % (94 * 2) + 0x31;
        if (*code2 > 0x7e) *code2 += 0x12;
        break;
    case CODEKIND_TGSPECIAL:
        *code1 = _HGR1st;
        *code2 = codeidx->index + _HGR2nd;
        break;
    default:
        *code1 = ' ', *code2 = ' ';
        break;
    }
}
