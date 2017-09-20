#pragma once
#include "stdint.h"
#include "MultiBoot.h"

extern void __cdecl  InitializeConstructors();

void SetInterruptVector();
void InitFloppyDrive();

bool InitKeyboard();
	
bool InitHardDrive();
void TestHardDrive();

bool InitGraphics(multiboot_info* pInfo);

bool DumpSystemInfo(multiboot_info* pBootInfo);

