#pragma once
#include "stdint.h"

extern void _cdecl kKeyboardHandler();
extern void __cdecl  InitializeConstructors();

void SetInterruptVector();
void InitializeFloppyDrive();
void CreateCentralSystem();
void CreateTestKernelProcess();

bool InitKeyboard();
bool InitHardDrive();
bool DumpSystemInfo();

