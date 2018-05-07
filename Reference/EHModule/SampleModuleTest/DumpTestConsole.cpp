// SampleModuleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "EHModuleManager.h"
#include "BaseClass.h"
#include "ExtensionClass.h"

struct PureCallBase 
{ 
	PureCallBase() { mf(); } 
	void mf() 
	{ 
		pvf(); 
	} 
	virtual void pvf() = 0; 
};

struct PureCallExtend : public PureCallBase
{ 
	PureCallExtend() {}
	virtual void pvf() {}
};

#include <crtdbg.h>

int MyReportHook(int reportType, char *message, int *returnValue)
{
	printf("런타임 에러 발생\n");
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	CEHModuleManager ModuleManger;
	//if(FALSE == ModuleManger.Install(DL_MY_HANDLER_STACKOVERFLOW, EH_BUGTRAP))
	if(FALSE == ModuleManger.Install(DL_MY_HANDLER_STACKOVERFLOW, EH_BUGTRAP_MIX))
	{
		printf("예외 핸들러 설치 실패\n");
	}

	_CrtSetReportHook(MyReportHook);

////////////////////////////////////////////////////////////////////////
//Generic Error
////////////////////////////////////////////////////////////////////////
	/*int* pPoint = 0;
	*pPoint = 1234;*/

////////////////////////////////////////////////////////////////////////
//CRT Error
////////////////////////////////////////////////////////////////////////
	TCHAR szData[100000] = L"SampleChatStringDataNoEndSpace!!";
	TCHAR szTargetBuffer[10] = {0,};
	_tcsncpy_s(szTargetBuffer, szData, 10);

////////////////////////////////////////////////////////////////////////
//Out Of Memory
////////////////////////////////////////////////////////////////////////
	//CExtensionClass* pExtensionClass = new CExtensionClass();

	//pExtensionClass->ProcessOutofMemory();

////////////////////////////////////////////////////////////////////////
//Heap Corruption
////////////////////////////////////////////////////////////////////////
	/*CBaseClass* pBaseClass = new CBaseClass();

	CExtensionClass* pExtensionClass = static_cast<CExtensionClass*>(pBaseClass);

	pExtensionClass->SetExtensionVar(12345678);
	pExtensionClass->ProcessHeapCorruption();	

	delete pBaseClass;*/

////////////////////////////////////////////////////////////////////////
//Pure Function Call
////////////////////////////////////////////////////////////////////////
	//PureCallExtend Temp;

////////////////////////////////////////////////////////////////////////
//Stack Overflow
////////////////////////////////////////////////////////////////////////
	/*CExtensionClass* pExtensionClass = new CExtensionClass();

	pExtensionClass->ProcessStackOverFlow();	

	delete pExtensionClass;*/

////////////////////////////////////////////////////////////////////////
//STATUS_ARRAY_BOUNDS_EXCEEDED : 확인해 볼 필요 있음
////////////////////////////////////////////////////////////////////////

	return 0;
}
