#pragma once
#include "stdint.h"
#include "MultiBoot.h"
#include "vesa.h"

extern void __cdecl  InitializeConstructors();

void SetInterruptVector();
void InitFloppyDrive();

bool InitKeyboard();
	
bool InitHardDrive();
void TestHardDrive();

bool InitGraphics(VesaModeInfo* pInfo);

bool DumpSystemInfo(multiboot_info* pBootInfo);

