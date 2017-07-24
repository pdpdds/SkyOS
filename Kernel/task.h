#pragma once
#include "header.h"
#include "VirtualMemoryManager.h"


extern int createThread    (int (*entry) (void), uint32_t stackBase);
extern int terminateThread (thread* handle);

class Process;

extern "C"
{
	void TerminateProcess();
	uint32_t MemoryAlloc(size_t size);
	void MemoryFree(void* p);
	void CreateDefaultHeap();
	void TerminateMemoryProcess();	
}

bool ValidatePEImage(void* image);