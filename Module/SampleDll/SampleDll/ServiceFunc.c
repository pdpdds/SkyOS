#include "ServiceFunc.h"

int k = 10;
int AddNumbers(int a, int b)
{
	return a + b + k;
}

DLLInterface g_Interface =
{
	AddNumbers,
};

__declspec(dllexport) const DLLInterface* GetDLLInterface()
{
	return &g_Interface;
}