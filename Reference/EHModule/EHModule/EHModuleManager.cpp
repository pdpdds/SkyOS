#include "StdAfx.h"
#include "EHHandlerSelector.h"
#include "EHModuleManager.h"
#include "EHExceptionHandlerDispatcher.h"

CEHModuleManager::CEHModuleManager(void)
{
}

CEHModuleManager::~CEHModuleManager(void)
{
}

BOOL CEHModuleManager::RegisterHandler()
{
	CEHExceptionHandlerDispatcher::GetInstance()->RegisterExceptionHandler(EH_MINIDUMP, MiniDumpHandler);
	CEHExceptionHandlerDispatcher::GetInstance()->RegisterExceptionHandler(EH_BUGTRAP, BugTrapHandler);	
	CEHExceptionHandlerDispatcher::GetInstance()->RegisterExceptionHandler(EH_USER_DEFINED, UserDefinedHandler);
	CEHExceptionHandlerDispatcher::GetInstance()->RegisterExceptionHandler(EH_BUGTRAP_MIX, BugTrapMixHandler);

	return TRUE;
}

BOOL CEHModuleManager::SetExceptionHandler(enumDumpDetectionLevel eLevel, enumExceptionHandler eExceptionHandler)
{
	CEHExceptionHandlerDispatcher::GetInstance()->ProcessExceptionHandler(eExceptionHandler, eLevel);

	return TRUE;
}

BOOL CEHModuleManager::Install(enumDumpDetectionLevel eLevel, enumExceptionHandler eExceptionHandler)
{
	if(eLevel >= DL_MAX || eExceptionHandler >= EH_MAX)
		return FALSE;

	if(FALSE == RegisterHandler())
		return FALSE;

	if(FALSE == SetExceptionHandler(eLevel, eExceptionHandler))
		return FALSE;

	return TRUE;
}