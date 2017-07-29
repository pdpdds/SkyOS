#pragma once
#include "stdint.h"
#include "MultiBoot.h"

extern void _cdecl kKeyboardHandler();
extern void __cdecl  InitializeConstructors();

void SetInterruptVector();
void InitFloppyDrive();

bool InitKeyboard();
	
bool InitHardDrive();


bool InitGraphics(multiboot_info* pInfo);

bool DumpSystemInfo();

