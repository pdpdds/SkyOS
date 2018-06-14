// TestEngine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h> 
#include <windows.h> 
#include "MapFile.h"
#include "MapFileSymbol.h"
#include "Undecorate.h"
#include "SkyMockInterface.h"

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef void (*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_MapFileReader*(*PGetDebugEngineDLL)();


void* GetModuleFunction(HINSTANCE handle, const char* func_name)
{
	return (void*)GetProcAddress(handle, func_name);
}

int main()
{
	HINSTANCE dllHandle = NULL;
	
	//디버그엔진 모듈을 로드한다.
	dllHandle = LoadLibrary("DebugEngine.dll");
	char* fileName = "SkyOS32.map";
	
	//디버그엔진 모듈이 익스포트하는 SetSkyMockInterface 함수와 GetDebugEngineDLL 함수를 얻어낸다.
	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetDebugEngineDLL GetDebugEngineDLLInterface = (PGetDebugEngineDLL)GetModuleFunction(dllHandle, "GetDebugEngineDLL");

	if (!GetDebugEngineDLLInterface || !SetSkyMockInterface)
	{
		printf("GetDebugEngineDLL Aquired failed!\n");
		return 0;
	}

	//SetSkyMockInterface 함수를 사용해서 디버그엔진 모듈에 파일인터페이스와 입출력, 화면출력 인터페이스를 제공한다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
	
	//디버그엔진으로 부터 맴파일리더 인터페이스를 얻어온다.
	I_MapFileReader* pMapReader = GetDebugEngineDLLInterface();

	if(pMapReader == nullptr)
	{
		printf("MapReader Creation Fail!\n");
		return 0;
	}

	//디버그엔진에 맵파일을 읽어들인다.
	pMapReader->readFile(fileName);

	//SkyOS 커널이 로드되는 기준주소는 0x100000이다.
	pMapReader->setLoadAddress(0x100000);

	int lineNumber = 0;
	DWORD resultAddress = 0;
	char function[256];
	char moduleName[256];
	char fileName2[256];

	//주소로부터 모듈이름, 함수 이름등을 얻어낸다.
	//라인번호를 얻어내기 위해서는 COD 파일이 필요하다.
	int result = pMapReader->getAddressInfo(0x00100df0,
		moduleName, fileName2, lineNumber, function, resultAddress);

	pMapReader->SimpleUndecorateCPP("?TraceStackWithSymbol@SkyDebugger@@QAEXI@Z", fileName2, 256);
	return 0;
}

