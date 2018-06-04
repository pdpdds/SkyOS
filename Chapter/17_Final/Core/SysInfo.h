#pragma once
#include "windef.h"
#include "SysError.h"

DWORD GetLastError();
DWORD SetLastError(DWORD dwErrorCode);
