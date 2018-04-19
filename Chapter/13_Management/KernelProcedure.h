#pragma once
#include "windef.h"

DWORD WINAPI SystemConsoleProc(LPVOID parameter);
DWORD WINAPI WatchDogProc(LPVOID parameter);
DWORD WINAPI ProcessRemoverProc(LPVOID parameter);
DWORD WINAPI ProcessRemoverProc2(LPVOID parameter);