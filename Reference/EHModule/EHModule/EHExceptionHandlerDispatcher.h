#pragma once
#include "EHEnum.h"
#include <MAP>

class CEHExceptionHandlerDispatcher
{
	typedef void (* HandlerSelector)(enumDumpDetectionLevel eLevel);
	typedef std::map<enumExceptionHandler, HandlerSelector> ExceptionHandlerList;

public:	
	virtual ~CEHExceptionHandlerDispatcher(void);

	static CEHExceptionHandlerDispatcher* GetInstance()
	{
		if(NULL == m_pExceptionHandlerDispatcher)
		{
			m_pExceptionHandlerDispatcher = new CEHExceptionHandlerDispatcher();
		}

		return m_pExceptionHandlerDispatcher;
	}

	void RegisterExceptionHandler(enumExceptionHandler eHandler, HandlerSelector Handler);
	BOOL ProcessExceptionHandler(enumExceptionHandler eHandler, enumDumpDetectionLevel eLevel);

protected:

private:
	CEHExceptionHandlerDispatcher(void);

	ExceptionHandlerList m_mapExceptionHandlerList;
	static CEHExceptionHandlerDispatcher* m_pExceptionHandlerDispatcher;
};