#pragma once
#include "SkyOS.h"

extern void __cdecl InitializeConstructors();
void SetInterruptVector();
void InitContext(multiboot_info* bootinfo);
void InitHardware();
bool InitMemoryManager(multiboot_info* bootinfo);

void kmain(unsigned long, unsigned long, uint32_t imageBase);
