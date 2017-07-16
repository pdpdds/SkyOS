#pragma once
#include "StdIntTypes.h"

void DebugPutc(unsigned char c);
void DebugClrScr(const uint8_t c);
void DebugPuts(char* str);
extern "C" int DebugPrintf(const char* str, ...);
unsigned DebugSetColor(const unsigned c);
void DebugGotoXY(unsigned x, unsigned y);
void DebugGetXY(unsigned* x, unsigned* y);
int DebugGetHorz();
int DebugGetVert();

void itoa_s(int i, unsigned base, char* buf);