#include "SkyOS.h"

DWORD dw_SysLastError = 0;
CPUID_DATA sysCPUBasicInformation[CPUID_MAX_BASIC_LEVEL];
CPUID_DATA sysCPUExtendedInformation[CPUID_MAX_EXTENDED_LEVEL];

DWORD GetLastError()
{
	//printf ("\n\r KE GetLastError %X", dw_SysLastError );
	return dw_SysLastError;
}
DWORD SetLastError(DWORD dwErrorCode)
{
	return dw_SysLastError = dwErrorCode;
}

