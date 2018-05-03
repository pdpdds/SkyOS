// ModuleRelocation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MDUMP.h"

__declspec(dllexport) void CrashFunction()
{
	printf("dll crash function enter\n");

	char* pNullPointer = 0;
	*pNullPointer = 'a';

	printf("dll crash function leave\n");
}

typedef int (__cdecl  *fnPointer_t) ();
fnPointer_t fnPointer;


int _tmain(int argc, _TCHAR* argv[])
{
	
	HMODULE hModule;	

	MiniDumper Dumper("ModuleRelocation");

	if (hModule = LoadLibrary(_T("SampleDll.dll")))
	{
		if (hModule = LoadLibrary(_T("SampleDll2.dll")))
		{			
			fnPointer = (fnPointer_t)GetProcAddress(hModule, "fnSampleDll");
			fnPointer = (fnPointer_t)GetProcAddress(hModule, "AAA");
		}
	}

	if (NULL != fnPointer)
	{
		fnPointer();
	}

	return 0;
}


