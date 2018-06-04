#include "SkyOS.h"



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
	SkyConsole::Print("cccPure Function Called!!\n");
	return 0;
}



