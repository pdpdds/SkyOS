#include "ServiceFunc.h"

int AddNumbers(int a, int b)
{
	return a + b;
}

DLLInterface g_Interface =
{
	AddNumbers,
};

__declspec(dllexport) const DLLInterface* GetDLLInterface()
{
	return &g_Interface;
}