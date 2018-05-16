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
	
	dllHandle = LoadLibrary("DebugEngine.dll");
	char* fileName = "SkyOS32.map";
	
	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetDebugEngineDLL GetDebugEngineDLLInterface = (PGetDebugEngineDLL)GetModuleFunction(dllHandle, "GetDebugEngineDLL");

	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
	
	if (!GetDebugEngineDLLInterface)
	{
		printf("GetDebugEngineDLL Aquired failed!\n");
		return 0;
	}

	I_MapFileReader* pMapReader = GetDebugEngineDLLInterface();

	if(pMapReader == nullptr)
	{
		printf("MapReader Creation Fail!\n");
		return 0;
	}

	pMapReader->readFile(fileName);
	pMapReader->setLoadAddress(0x100000);

	int lineNumber = 0;
	DWORD resultAddress = 0;
	char function[256];
	char moduleName[256];
	char fileName2[256];

	int result = pMapReader->getAddressInfo(0x00100df0,
		moduleName, fileName2, lineNumber, function, resultAddress);
	
	return 0;
}

