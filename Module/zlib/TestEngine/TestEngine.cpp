// TestEngine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h> 
#include <windows.h> 
#include "SkyMockInterface.h"
#include "I_Compress.h"

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef void (*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_Compress* (*PGetEasyCompress)();


void* GetModuleFunction(HINSTANCE handle, const char* func_name)
{
	return (void*)GetProcAddress(handle, func_name);
}

void TestEasyZLib(I_Compress* pCompress);

int main()
{
	HINSTANCE dllHandle = NULL;
		
	dllHandle = LoadLibrary("zlib.dll");	
	
	//디버그엔진 모듈이 익스포트하는 SetSkyMockInterface 함수와 GetDebugEngineDLL 함수를 얻어낸다.
	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetEasyCompress GetEasyCompress = (PGetEasyCompress)GetModuleFunction(dllHandle, "GetEasyCompress");

	//SetSkyMockInterface 함수를 사용해서 디버그엔진 모듈에 파일인터페이스와 입출력, 화면출력 인터페이스를 제공한다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
	I_Compress* pCompress = GetEasyCompress();
	TestEasyZLib(pCompress);
	
	return 0;
}

char easyTestBuffer[] = "Sky OS Compression Test!!";
void TestEasyZLib(I_Compress* pCompress)
{
	char* destBuffer = new char[256];
	long destBufferLen = 256;
	long nSrcLen = sizeof(easyTestBuffer);

	char* decompressedBuffer = new char[256];
	long decompressedLen = 256;

	memset(destBuffer, 0, 256);
	memset(decompressedBuffer, 0, 256);

	printf("text : %s\n", easyTestBuffer);

	//압축한다.
	if (0 != pCompress->Compress((unsigned char*)destBuffer, &destBufferLen, ( unsigned char*)easyTestBuffer, nSrcLen))
	{
		printf("easyzlib test fail!!");
	}
	printf("Compressed : Src Size %d, Dest Size %d\n", nSrcLen, destBufferLen);

	//압축을 해제한다. 
	if (0 != pCompress->Decompress((unsigned char*)decompressedBuffer, &decompressedLen, ( unsigned char*)destBuffer, destBufferLen))
	{
		printf("easyzlib test fail!!");
	}
	printf("Decompressed : Src Size %d, Dest Size %d\n", destBufferLen, decompressedLen);
	printf("result : %s\n", decompressedBuffer);

	delete destBuffer;
	delete decompressedBuffer;
}