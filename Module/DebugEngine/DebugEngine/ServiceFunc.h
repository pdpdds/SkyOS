#pragma once

typedef struct DLLInterface
{
	int(*AddNumbers)(int a, int b);	
} DLLInterface;



__declspec(dllexport) const DLLInterface* GetDLLInterface();

#define kmalloc(a) g_mockInterface.g_allocInterface.sky_kmalloc(a);
#define kfree(p) g_mockInterface.g_allocInterface.sky_kfree(p);