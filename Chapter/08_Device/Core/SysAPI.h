#pragma once
#include "windef.h"
#include "PIT.h"
#include "SkyConsole.h"

#define MAX_SYSCALL 8

void *operator new(size_t size);
void *operator new[](size_t size);
//void *operator new(size_t, void *p);
//void *operator new[](size_t, void *p);



void operator delete(void *p);
void operator delete(void *p, size_t size);
void operator delete[](void *p);
void operator delete[](void *p, size_t size);


int __cdecl _purecall();

#ifdef __cplusplus
extern "C" {
#endif

#define CPUID_MAX_BASIC_LEVEL       10
#define CPUID_MAX_EXTENDED_LEVEL    10

#pragma pack (push, 1)
	typedef struct
	{
		UINT32 EAX;
		UINT32 EBX;
		UINT32 EDX;
		UINT32 ECX;
	}CPUID_DATA;
#pragma pack (pop)

#ifdef __cplusplus
}
#endif