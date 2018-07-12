#pragma once
#include "windef.h"

DWORD WINAPI SystemConsoleProc(LPVOID parameter);
DWORD WINAPI WatchDogProc(LPVOID parameter);
DWORD WINAPI ProcessRemoverProc(LPVOID parameter);

DWORD WINAPI SystemGUIProc(LPVOID parameter);
DWORD WINAPI ConsoleGUIProc(LPVOID parameter);
DWORD WINAPI ConsoleDebugGUIProc(LPVOID parameter);
DWORD WINAPI GUIWatchDogProc(LPVOID parameter);

DWORD WINAPI SystemIdle(LPVOID parameter);