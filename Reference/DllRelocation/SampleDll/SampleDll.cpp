// SampleDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "stdio.h"
#include "SampleDll.h"


// This is an example of an exported variable
SAMPLEDLL_API int nSampleDll=0;

// This is an example of an exported function.
SAMPLEDLL_API int fnSampleDll(void)
{
	printf("dll crash function enter\n");

	//char* pNullPointer = 0;
	//*pNullPointer = 'a';

	printf("dll crash function leave\n");

	return 42;
}

// This is the constructor of a class that has been exported.
// see SampleDll.h for the class definition
CSampleDll::CSampleDll()
{
	return;
}
