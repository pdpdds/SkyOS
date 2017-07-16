#pragma once
#include "StdIntTypes.h"

#ifdef _MSC_VER
#define interrupt __declspec (naked)
#else
#define interrupt
#endif
void _cdecl	interruptdone(unsigned int intno);

#define far
#define near

void OutPortByte(ushort port, uchar value);
uchar InPortByte(ushort port);
void OutPortWord(ushort port, ushort value);
ushort InPortWord(ushort port);

