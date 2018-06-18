// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.

#include "windef.h"
#include "SkyMockInterface.h"

#include "EasyCompress.h"
extern "C" __declspec(dllexport) I_Compress* GetEasyCompress()
{
	return (I_Compress*)new EasyCompress();
}

#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0
#define DLL_THREAD_ATTACH 2
#define DLL_THREAD_DETACH 3

extern void _cdecl InitializeConstructors();
extern void _cdecl Exit();

bool __stdcall DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		
	}
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


extern "C" bool WINAPI _DllMainCRTStartup(
	HANDLE  hDllHandle,
	DWORD   dwReason,
	LPVOID  lpreserved
)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// set up our minimal cheezy atexit table
		//_atexit_init();

		// Call C++ constructors
		//_initterm(__xc_a, __xc_z);

		InitializeConstructors();
	}

	bool retcode = DllMain(hDllHandle, dwReason, lpreserved);

	if (dwReason == DLL_PROCESS_DETACH)
	{
		Exit();
	}

	return retcode;
}
