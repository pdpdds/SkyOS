/*
 *      File C_COMMON.C
 *      Code-to-index Conversion Function Set
 *      for Common Combined Hangul Code
 *      '95.4.27-28,30
 *      Written by Inkeon Lim
 */


#include "hancode.h"


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2);
static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx);


code_t _code_COMMON = {code_getidx, code_getcode};


static void code_getidx(codeidx_t *codeidx, byte code1, byte code2)
{
    code__getidx(&_codetbl_COMMON, codeidx, code1, code2);
}

static void code_getcode(byte *code1, byte *code2, codeidx_t *codeidx)
{
    code__getcode(&_codetbl_COMMON, code1, code2, codeidx);
}
