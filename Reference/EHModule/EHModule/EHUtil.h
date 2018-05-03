#pragma once
#include "DbgHelp.h"
#include <Process.h>

BOOL RecoverSetUnhandledExceptionFilter();

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
BOOL PreventSetUnhandledExceptionFilter();

unsigned __stdcall CreateMiniDump(void* pArguments);
LONG WINAPI MyMiniDumpHandler(PEXCEPTION_POINTERS pException);
LONG WINAPI MyBugTrapMixHandler(PEXCEPTION_POINTERS pException);

void New_OutOfMemory_Handler();
unsigned __stdcall CreateOutofMemoryLog(void* pArguments);

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
										 );

//////////////////////////////////////////////////////////////
//User Defined Handler
//////////////////////////////////////////////////////////////
void InvalidParameterHandler (PCTSTR expression,
							  PCTSTR function,
							  PCTSTR file,
							  unsigned int line,
							  uintptr_t pReserved);

void PureFunctionCallHandler();

BOOL GetLogicalAddress(PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD &offset);
LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);

//////////////////////////////////////////////////////////////
//유틸리티 함수
//////////////////////////////////////////////////////////////
void GetOutofMemoryLogPath(TCHAR* pPathName);
void GetUserDefinedLogPath(TCHAR* pPathName);
void AddOutofMemoryLogHeader(DWORD dwCurrentThreadId);