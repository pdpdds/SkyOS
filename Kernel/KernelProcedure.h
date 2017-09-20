#pragma once
#include "windef.h"

DWORD WINAPI SampleLoop(LPVOID parameter);

DWORD WINAPI SystemConsoleProc(LPVOID parameter);
DWORD WINAPI WatchDogProc(LPVOID parameter);
DWORD WINAPI ProcessRemoverProc(LPVOID parameter);