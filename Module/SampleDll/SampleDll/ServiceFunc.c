#include "ServiceFunc.h"

int k = 10;
int AddNumbers(int a, int b)
{
	return a + b + 10;
}

DLLInterface g_Interface =
{
	AddNumbers,
};

__declspec(dllexport) const DLLInterface* GetDLLInterface()
{
	return &g_Interface;
}