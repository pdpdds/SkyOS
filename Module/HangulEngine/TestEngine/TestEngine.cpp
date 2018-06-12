// TestEngine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h> 
#include <windows.h> 
#include "SkyMockInterface.h"
#include "I_Hangul.h"

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef void (*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_Hangul* (*PGetHangulEngine)();


void* GetModuleFunction(HINSTANCE handle, const char* func_name)
{
	return (void*)GetProcAddress(handle, func_name);
}

int main()
{
	HINSTANCE dllHandle = NULL;
	
	//디버그엔진 모듈을 로드한다.
	dllHandle = LoadLibrary("HangulEngine.dll");	
	
	//디버그엔진 모듈이 익스포트하는 SetSkyMockInterface 함수와 GetDebugEngineDLL 함수를 얻어낸다.
	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetHangulEngine GetHangulEngine = (PGetHangulEngine)GetModuleFunction(dllHandle, "GetHangulEngine");

	//SetSkyMockInterface 함수를 사용해서 디버그엔진 모듈에 파일인터페이스와 입출력, 화면출력 인터페이스를 제공한다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);

	I_Hangul* pHangul = GetHangulEngine();
	char* vram = new char[1024 * 768];
	pHangul->Initialize();
	pHangul->PutFonts(vram, 1024, 0, 0, 0xffffffff, (byte*)("한글 a출력"));	
	return 0;
}

