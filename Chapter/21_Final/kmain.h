#pragma once
#include "SkyOS.h"

extern void __cdecl InitializeConstructors();
void SetInterruptVector();

void kmain(unsigned long, unsigned long);
