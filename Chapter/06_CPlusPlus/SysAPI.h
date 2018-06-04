#pragma once
#include "SkyAPI.h"

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete(void *p, size_t size);
void operator delete[](void *p);

int __cdecl _purecall();
