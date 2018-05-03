#pragma once
#include "EHEnum.h"

class CEHModuleManager
{
public:
	CEHModuleManager(void);
	virtual ~CEHModuleManager(void);

	BOOL Install(enumDumpDetectionLevel eLevel, enumExceptionHandler eExceptionHandler);
	BOOL RegisterHandler();

protected:
	BOOL SetExceptionHandler(enumDumpDetectionLevel eLevel, enumExceptionHandler eExceptionHandler);

private:
};