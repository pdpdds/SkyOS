#pragma once
#include "stdint.h"
#include "defines.h"
#include "string.h"
#include "sprintf.h"
#include "MultiBoot.h"
#include "SkyConsole.h"

void kmain(unsigned long, unsigned long);
extern "C" void SwitchAndExecute64bitKernel(int pml4EntryAddress, int kernelEntry, int bootinfo);
uint32_t GetModuleEnd(multiboot_info* bootinfo);
bool Boot64BitMode(multiboot_info_t* pBootInfo, char* szKernelName);
bool Boot32BitMode(unsigned long magic, multiboot_info_t* pBootInfo, char* szKernelName);
bool Is64BitSwitchPossible();
Module* FindModule(multiboot_info_t* pInfo, const char* szFileName);