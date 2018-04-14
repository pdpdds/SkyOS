#include "sysapi.h"
#include "kheap.h"
#include "idt.h"
#include "SkyConsole.h"

void *operator new(size_t size)
{
	return (void *)kmalloc(size);
}

void * __cdecl operator new[](size_t size)
{
	return (void *)kmalloc(size);
}

void __cdecl operator delete(void *p)
{
	for (;;);
	kfree(p);
}

void operator delete(void *p, size_t size)
{
	kfree(p);
}

void operator delete[](void *p)
{
	kfree(p);
}

int __cdecl _purecall()
{
	SkyConsole::Print("Pure Function Called!!\n");
	return 0;
}
