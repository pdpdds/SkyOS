#include "StdAfx.h"
#include "EHExceptionHandlerDispatcher.h"

CEHExceptionHandlerDispatcher* CEHExceptionHandlerDispatcher::m_pExceptionHandlerDispatcher = NULL;

using namespace std;

CEHExceptionHandlerDispatcher::CEHExceptionHandlerDispatcher(void)
{

}

CEHExceptionHandlerDispatcher::~CEHExceptionHandlerDispatcher(void)
{
	m_mapExceptionHandlerList.clear();
}

void CEHExceptionHandlerDispatcher::RegisterExceptionHandler(enumExceptionHandler eHandler, HandlerSelector Handler)
{
	m_mapExceptionHandlerList.insert(ExceptionHandlerList::value_type(eHandler, Handler));
}

BOOL CEHExceptionHandlerDispatcher::ProcessExceptionHandler(enumExceptionHandler eHandler, enumDumpDetectionLevel eLevel)
{
	ExceptionHandlerList::iterator iter = m_mapExceptionHandlerList.find(eHandler);

	if(iter != m_mapExceptionHandlerList.end())
	{
		iter->second(eLevel);
		return TRUE;
	}

	return FALSE;
}