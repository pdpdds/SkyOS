// SampleDll2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "stdio.h"
#include "SampleDll2.h"


// This is an example of an exported variable
SAMPLEDLL2_API int nSampleDll2=0;

// This is an example of an exported function.
extern "C" SAMPLEDLL2_API int AAA(void)
{
	printf("dll crash function enter\n");

	char* pNullPointer = 0;
	*pNullPointer = 'a';

	printf("dll crash function leave\n");

	return 42;
}

// This is the constructor of a class that has been exported.
// see SampleDll2.h for the class definition
CSampleDll2::CSampleDll2()
{
	return;
}
