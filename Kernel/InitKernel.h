#pragma once
#include "stdint.h"

extern void _cdecl kKeyboardHandler();
extern void __cdecl  InitializeConstructors();

void SetInterruptVector();
void InitFloppyDrive();
void CreateCentralSystem();

bool InitKeyboard();
bool InitHardDrive();
bool DumpSystemInfo();

