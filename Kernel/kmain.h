#pragma once
#include "stdint.h"
#include "header.h"
#include "exception.h"
#include "string.h"
#include "sprintf.h"
#include "SkyConsole.h"
#include "HAL.h"
#include "SkyAPI.H"
#include "IDT.h"
#include "GDT.h"
#include "PIT.h"
#include "PIC.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"
#include "kheap.h"
#include "ZetPlane.h"
#include "sysapi.h"
#include "tss.h"
#include "ProcessManager.h"
#include "ConsoleManager.h"
#include "List.h"
#include "KernelProcedure.h"
#include "InitKernel.h"
#include "StdVGA.H"
#include "fat32.h"
#include "fat12.h"
#include "RAMDisk.h"

void kmain(unsigned long, unsigned long);
bool InitMemoryManager(multiboot_info* bootinfo, uint32_t kernelSize);
void StartConsoleSystem();
void JumpToNewKernelEntry(int entryPoint, unsigned int procStack);