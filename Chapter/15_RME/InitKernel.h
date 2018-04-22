#pragma once
#include "stdint.h"
#include "MultiBoot.h"

extern void __cdecl InitializeConstructors();

void SetInterruptVector();

bool InitKeyboard();