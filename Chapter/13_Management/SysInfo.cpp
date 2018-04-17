#include "SysInfo.h"

DWORD dw_SysLastError = 0;

DWORD GetLastError()
{
	//printf ("\n\r KE GetLastError %X", dw_SysLastError );
	return dw_SysLastError;
}
DWORD SetLastError(DWORD dwErrorCode)
{
	return dw_SysLastError = dwErrorCode;
}