// TestEngine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h> 
#include <windows.h> 
#include "SkyMockInterface.h"
#include "I_Compress.h"
#include <iostream>

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef void (*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_Compress* (*PGetZlibCompress)();


void* GetModuleFunction(HINSTANCE handle, const char* func_name)
{
	return (void*)GetProcAddress(handle, func_name);
}

int main()
{
	HINSTANCE dllHandle = NULL;

	//디버그엔진 모듈을 로드한다.
	dllHandle = LoadLibrary("zlib128.dll");

	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetZlibCompress GetZlibCompress = (PGetZlibCompress)GetModuleFunction(dllHandle, "GetZlibCompress");

	//SetSkyMockInterface 함수를 사용해서 디버그엔진 모듈에 파일인터페이스와 입출력, 화면출력 인터페이스를 제공한다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
	I_Compress* pCompress = GetZlibCompress();

	const  int BUF = 1024;
	const  int DBUF = BUF * 2 + 13;

	byte raw_data[] = "Hello Zlib 1.28!!!";
	byte deflate_data[DBUF];

	unsigned long raw_size = strlen((const  char*)raw_data);
	unsigned long deflate_size = DBUF;

	//compress 사용하기
	{
		pCompress->Compress(deflate_data, (long*)&deflate_size, raw_data, raw_size);
		std::cout << "Raw Data Size:" << raw_size << std::endl;
		std::cout << "Deflate Data Size:" << deflate_size << std::endl;
	}

	byte inflate_data[BUF];
	unsigned long inflate_size = BUF;
	//uncompress 사용하기
	{
		pCompress->Decompress(inflate_data, (long*)&inflate_size, deflate_data, deflate_size);
		std::cout << "Deflate Data Size:" << deflate_size << std::endl;
		std::cout << "Inflate Size:" << inflate_size << std::endl;
		inflate_data[inflate_size] = NULL;
		std::cout << "original data : " << (const  char*)inflate_data << std::endl;
	}


	return 0;
}

