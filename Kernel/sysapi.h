#pragma once
#include "SkyAPI.h"

#define MAX_SYSCALL 7

static void* _syscalls[] = {

	printf,
	TerminateProcess,
	MemoryAlloc,
	MemoryFree,
	CreateDefaultHeap,
	GetTickCount,
	CreateThread,
};

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete(void *p, size_t size);
int __cdecl _purecall();
void operator delete[](void *p);


void InitializeSysCall();