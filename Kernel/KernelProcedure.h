#pragma once
#include "windef.h"

void NativeConsole();
DWORD WINAPI SystemEntry(LPVOID parameter);
DWORD WINAPI SampleLoop(LPVOID parameter);
DWORD WINAPI TaskProcessor(LPVOID parameter);
DWORD WINAPI TestKernelProcess(LPVOID parameter);