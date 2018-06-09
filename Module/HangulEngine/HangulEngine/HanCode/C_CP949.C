/*
 *      File C_CP949.C
 *      Code-to-index Conversion Function Set
 *      for CP949 Extended KS Fixed Code
 *      '07.7.26
 *      Rewritten by fontutil
 */



#include "extkey.h"
#include "hancode.h"


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2);
static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx);


code_t _code_CP949 = {code_getidx, code_getcode};


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2)
{
    word common;
    byte initial, medial, final;

    codeidx->code1 = code1, codeidx->code2 = code2;

	if (code1 < 0x81 || code2 < 0x41 || code2 > 0xfe) 
		codeidx->kind = CODEKIND_NOCODE;
	else if ((code1 >= 0xca && code1 <= 0xfd) && (code2 >= 0xa1 && code2 <= 0xfe)) {
		codeidx->kind = CODEKIND_HANJA;
		codeidx->index = (code1 - 0xca) * 94 + (code2 - 0xa1);
    } else if ((code1 >= 0xa1 && code1 <= 0xac) && (code2 >= 0xa1 && code2 <= 0xfe)) {
		codeidx->kind = CODEKIND_SPECIAL;
		codeidx->index = (code1 - 0xa1) * 94 + (code2 - 0xa1);
	} else {
		common = _cnvtbl_CP949HAN[code1 - 0x81][code2 - 0x41];
		if (common == 0)
			codeidx->kind = CODEKIND_NOCODE;
		else {
			codeidx->kind = CODEKIND_HANGUL;
	
			code1 = (byte)(common >> 8);
			code2 = (byte)common;

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
	}
}

static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx)
{
    const byte (*idx2code)[27 + 1] = _codetbl_COMMON.idx2code;
    word cp949;
    byte s[2];

    switch (codeidx->kind) {
    case CODEKIND_HANGUL:
        s[0] = 128 | (idx2code[INITIAL][codeidx->han.initial - H1_FC] << 2) |
               (idx2code[MEDIAL][codeidx->han.medial - H2_FC] >> 3);
        s[1] = (idx2code[MEDIAL][codeidx->han.medial - H2_FC] << 5) |
               idx2code[FINAL][codeidx->han.final - H3_FC];

        cp949 = _cnvtbl_COMMON[s[0] - 0x88][s[1] - 0x41];
        if (cp949 == 0) *code1 = ' ', *code2 = ' ';
        else {
            *code1 = (byte)(cp949 >> 8);
            *code2 = (byte)cp949;
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
    default:
        *code1 = ' ', *code2 = ' ';
        break;
    }
}
