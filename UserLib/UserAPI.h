#pragma once

typedef int uint32_t;
typedef unsigned int   u32int;

uint32_t GetTickCount();
void free(void *p);
u32int malloc(u32int sz);
void CreateHeap();
void TerminateProcess();
void printf(const char* szMsg, ...);
char GetKeyboardInput();

void sleep(int millisecond);

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
int __cdecl _purecall();
void operator delete[](void *p);
void operator delete(void *p, size_t size);
