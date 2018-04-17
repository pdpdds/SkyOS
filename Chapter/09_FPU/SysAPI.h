#pragma once
#include "SkyAPI.h"
#include "SkyConsole.h"

#define MAX_SYSCALL 1

static void* _syscalls[] = {

	kprintf,
	
};

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete(void *p, size_t size);
void operator delete[](void *p);

int __cdecl _purecall();

void InitializeSysCall();
