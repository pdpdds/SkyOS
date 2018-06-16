#include "intrinsic.h"
#include "kheap.h"
#include "SkyConsole.h"

void *operator new(size_t size)
{
	return (void *)kmalloc(size);
}

void *operator new(size_t, void *p)
{
	return p;
}

void *operator new[](size_t, void *p)
{
	return p;
}

void * __cdecl operator new[](size_t size)
{
	return (void *)kmalloc(size);
}

void __cdecl operator delete(void *p)
{
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

void operator delete[](void *p, size_t size)
{
	kfree(p);
}

int __cdecl _purecall()
{
	SkyConsole::Print("Pure Function Called!!\n");
	return 0;
}
