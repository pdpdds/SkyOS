/*
 *      File C__COMBI.C
 *      Code-to-index Conversion Function Set
 *      for All Kinds of Combined Hangul Codes
 *      '95.4.28-30
 *      Written by Inkeon Lim
 */


#include "extkey.h"
#include "hancode.h"


void code__getidx(codetbl_t *codetbl, codeidx_t *codeidx, byte code1,
                  byte code2)
{
    byte initial, medial, final;

    codeidx->kind = CODEKIND_HANGUL;

    codeidx->code1 = code1, codeidx->code2 = code2;

    initial = (code1 & 0x7c) >> 2;
    medial = ((code1 & 0x03) << 3) | (code2 >> 5);
    final = code2 & 0x1f;
    initial = codetbl->code2idx[INITIAL][initial];
    medial = codetbl->code2idx[MEDIAL][medial];
    final = codetbl->code2idx[FINAL][final];

    if (initial == NC || medial == NC || final == NC)
        codeidx->kind = CODEKIND_NOCODE;

    codeidx->han.initial = H1_FC + initial;
    codeidx->han.medial = H2_FC + medial;
    codeidx->han.final = H3_FC + final;
}

void code__getcode(codetbl_t *codetbl, byte *code1, byte *code2,
                   codeidx_t *codeidx)
{
    const byte (*idx2code)[27 + 1] = codetbl->idx2code;

    switch (codeidx->kind) {
    case CODEKIND_HANGUL:
        *code1 = 128 |
                 (idx2code[INITIAL][codeidx->han.initial - H1_FC] << 2) |
                 (idx2code[MEDIAL][codeidx->han.medial - H2_FC] >> 3);
        *code2 = (idx2code[MEDIAL][codeidx->han.medial - H2_FC] << 5) |
                 idx2code[FINAL][codeidx->han.final - H3_FC];
        break;
    default:
        *code1 = ' ', *code2 = ' ';
        break;
    }
}
