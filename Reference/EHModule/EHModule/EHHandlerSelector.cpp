#include "StdAfx.h"
#include "EHUtil.h"
#include "EHHandlerSelector.h"
#include <process.h>
#include "BugTrap.h"
#include <MAP>

using namespace std;


void MiniDumpHandler(enumDumpDetectionLevel eLevel)
{		
	SetUnhandledExceptionFilter(MyMiniDumpHandler);

	if(DL_MY_HANDLER == eLevel || DL_MY_HANDLER_STACKOVERFLOW == eLevel)
	{
		PreventSetUnhandledExceptionFilter();
	}

	std::set_new_handler(New_OutOfMemory_Handler);
}

void BugTrapMixHandler(enumDumpDetectionLevel eLevel)
{		
	SetUnhandledExceptionFilter(MyBugTrapMixHandler);

	if(DL_MY_HANDLER == eLevel || DL_MY_HANDLER_STACKOVERFLOW == eLevel)
	{
		PreventSetUnhandledExceptionFilter();
	}

	std::set_new_handler(New_OutOfMemory_Handler);
}

void BugTrapHandler(enumDumpDetectionLevel eLevel)
{	
	TCHAR szCurDirectory[MAX_PATH] = {0,};
	TCHAR* pEnd = NULL;
	GetModuleFileName(NULL, szCurDirectory, MAX_PATH);
	pEnd = _tcsrchr(szCurDirectory, _T('\\'));

	if(NULL == pEnd)
	{
		return;
	}

	*pEnd = _T('\0');
	pEnd = pEnd + 1;

	BT_InstallSehFilter();
	BT_SetAppName(pEnd);
	BT_SetReportFilePath(szCurDirectory);
	BT_SetActivityType(BTA_SAVEREPORT);
	BT_SetReportFormat(BTRF_TEXT);
	BT_SetFlags(BTF_DETAILEDMODE);

	if(DL_MY_HANDLER == eLevel || DL_MY_HANDLER_STACKOVERFLOW == eLevel)
	{
		BT_InterceptSUEF(GetModuleHandle(NULL), TRUE);
	}

	std::set_new_handler(New_OutOfMemory_Handler);
}

void UserDefinedHandler(enumDumpDetectionLevel eLevel)
{
	_CrtSetReportMode(_CRT_ASSERT, 0);

	_invalid_parameter_handler newHandler = InvalidParameterHandler;
	_invalid_parameter_handler oldHandler = _set_invalid_parameter_handler(newHandler);

	_set_purecall_handler(PureFunctionCallHandler);

	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	if(DL_MY_HANDLER == eLevel || DL_MY_HANDLER_STACKOVERFLOW == eLevel)
	{
		PreventSetUnhandledExceptionFilter();
	}

	std::set_new_handler(New_OutOfMemory_Handler);
}
