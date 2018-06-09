#include "ServiceFunc.h"
#include "SkyMockInterface.h"
#include "I_MapFileReader.h"

SkyMockInterface g_mockInterface;

extern "C" __declspec(dllexport) void SetSkyMockInterface(SKY_ALLOC_Interface allocInterface, 
														  SKY_FILE_Interface fileInterface, 
														  SKY_Print_Interface printInterface)
{
	g_mockInterface.g_allocInterface = allocInterface;
	g_mockInterface.g_fileInterface = fileInterface;
	g_mockInterface.g_printInterface = printInterface;
}


extern "C" __declspec(dllexport) I_MapFileReader* GetDebugEngineDLL()
{
	return nullptr;
}


void *operator new(size_t size)
{
	return (void*)kmalloc(size);
}

void* __cdecl operator new[](size_t size)
{
	return (void*)kmalloc(size);
}

void __cdecl operator delete(void *p)
{
	kfree(p);
}

void operator delete(void *p, size_t size)
{
	kfree(p);
}

void operator delete[](void *p, size_t size)
{
	kfree(p);
}


void operator delete[](void *p)
{
	kfree(p);
}

int __cdecl _purecall()
{
	return 0;
}