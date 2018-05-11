#pragma once

typedef struct DLLInterface
{
	int(*AddNumbers)(int a, int b);	
} DLLInterface;

__declspec(dllexport) const DLLInterface* GetDLLInterface();