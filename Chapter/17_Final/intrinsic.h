#pragma once

void *operator new(size_t size);
void *operator new[](size_t size);
void *operator new(size_t, void *p);
void *operator new[](size_t, void *p);

void operator delete(void *p);
void operator delete(void *p, size_t size);
void operator delete[](void *p);
void operator delete[](void *p, size_t size);

int __cdecl _purecall();