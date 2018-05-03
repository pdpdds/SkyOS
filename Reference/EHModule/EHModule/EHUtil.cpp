#include "StdAfx.h"
#include <time.h>
#include <process.h>
#include "BugTrap.h"
#include "EHEnum.h"
#include "EHUtil.h"
#include "EHStackWalker.h"
#include <fstream>
#include <new>

using namespace  std;

char g_pSetUnhandledFilterEntryValue[5] = {0,};

//////////////////////////////////////////////////////////////
//서비스용 메시지 박스 함수
//////////////////////////////////////////////////////////////
#define DECLARE_INIT_TCHAR_ARRAY(Name, Count) TCHAR Name[Count] = {TEXT('\0')};
#define DIMOF(Array) (sizeof(Array) /sizeof(Array[0]))

void ShowMessageBoxV(LPCTSTR pMessage, va_list argList)
{
	DECLARE_INIT_TCHAR_ARRAY(szMessage, 1025);
	DECLARE_INIT_TCHAR_ARRAY(szTitle, _MAX_PATH + 1);

	::wvsprintf(szMessage, pMessage, argList);
	::GetModuleFileName(NULL, szTitle, DIMOF(szTitle));
	HWND hwnd = ::GetActiveWindow();
	::MessageBox(hwnd, szMessage, szTitle, MB_OK | ((NULL == hwnd) ? MB_SERVICE_NOTIFICATION : 0));
}

void ShowMessageBox(LPCTSTR pMessage, ...)
{
	va_list argList;
	va_start(argList, pMessage);

	::ShowMessageBoxV(pMessage, argList);

	va_end(argList);
}

void New_OutOfMemory_Handler()
{
	DWORD dwCurrentThreadId = GetCurrentThreadId();
	
	AddOutofMemoryLogHeader(dwCurrentThreadId);

	HANDLE hCurrentThread = OpenThread(THREAD_GET_CONTEXT, FALSE, dwCurrentThreadId);

	HANDLE hThread = (HANDLE)_beginthreadex(0, NULL, CreateOutofMemoryLog, &hCurrentThread, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	
	exit(0);
	//RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////
//Exception Unhandled Filter가 Overriding되는 것을 막기 위한 루틴
//Kernel32.dll의 SetUnhandledExceptionFilter 함수의 진입점에 5바이트 점프코드를 써서
//MyDummySetUnhandledExceptionFilter가 호출되게 하고 이 함수에서는 어떠한 처리도 수행하지 않음
//결과적으로 CRT 함수 등의 set_invalid_parameter나 pure_function_call을 따로 구현해 줄 필요가 없게 된다.
////////////////////////////////////////////////////////////////////////////////////////
#ifndef _M_IX86
#error "The following code only works for x86!"
#endif
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(
	LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	printf("누군가가 호출했군\n");

	CEHStackWalker EHStackWalker;
	EHStackWalker.SetLogFilePath();

	EHStackWalker.ShowCallstack(GetCurrentThread());

	getchar();
	return NULL;
}

BOOL PreventSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));

	if (hKernel32  == NULL) return FALSE;

	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if(NULL == pOrgEntry) 
		return FALSE;

	memcpy(g_pSetUnhandledFilterEntryValue, pOrgEntry, 5 * sizeof(char));

	unsigned char newJump[ 100 ];
	DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;

	dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
	DWORD dwNewEntryAddr = (DWORD) pNewFunc;

	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

	newJump[ 0 ] = 0xE9;  // JMP absolute
	memcpy(&newJump[ 1 ], &dwRelativeAddr, sizeof(pNewFunc));
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);	

	return bRet;
}

////////////////////////////////////////////////////////////////////////////////////////
//Exception Unhandled Filter 함수가 동작하도록 복구하는 루틴
//검증된 모듈을 사용하고 싶고, 검증된 모듈에서 보다 확장된 기능을 추가하고 싶은 경우
//SetUnhandledFilter 함수가 제대로 동작하도록 원래대로 돌릴 필요가 있으며 그때 사용하는 함수
//FS 레지스터를 통해 예외함수 체인에 추가하는 방식으로 구현할 수도 있으나 잘되지 않아서....
////////////////////////////////////////////////////////////////////////////////////////
BOOL RecoverSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));

	if (hKernel32  == NULL) return FALSE;

	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if(NULL == pOrgEntry) 
		return FALSE;

	if(g_pSetUnhandledFilterEntryValue[0] == 0)
		return FALSE;

	//memcpy(pOrgEntry, g_pSetUnhandledFilterEntryValue, 5 * sizeof(char));
	
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, g_pSetUnhandledFilterEntryValue, 5 * sizeof(char), &bytesWritten);

	return bRet;
}
////////////////////////////////////////////////////////////////////////////////////////
//BugTrap Mix Handler
////////////////////////////////////////////////////////////////////////////////////////
BOOL ProcessBugTrapMix(EXCEPTION_POINTERS* pException)
{
	RecoverSetUnhandledExceptionFilter();
	
	TCHAR szCurDirectory[MAX_PATH] = {0,};
	TCHAR* pEnd = NULL;
	GetModuleFileName(NULL, szCurDirectory, MAX_PATH);
	pEnd = _tcsrchr(szCurDirectory, _T('\\'));

	if(NULL == pEnd)
	{
		return FALSE;
	}

	*pEnd = _T('\0');
	pEnd = pEnd + 1;

	BT_InstallSehFilter();
	BT_SetAppName(pEnd);
	BT_SetReportFilePath(szCurDirectory);
	BT_SetActivityType(BTA_SAVEREPORT);
	BT_SetReportFormat(BTRF_TEXT);
	BT_SetFlags(BTF_DETAILEDMODE);
	
	BT_InterceptSUEF(GetModuleHandle(NULL), TRUE);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
//My MiniDump Handler
////////////////////////////////////////////////////////////////////////////////////////
typedef struct _MinidumpInfo
{
	DWORD threadId;    
	EXCEPTION_POINTERS* pException;
} MinidumpInfo, *PMinidumpInfo;

BOOL ProcessMiniDump(EXCEPTION_POINTERS* pException)
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = NULL;						// find a better value for your app

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	TCHAR szDbgHelpPath[MAX_PATH];

	if (GetModuleFileName( NULL, szDbgHelpPath, MAX_PATH ))
	{
		TCHAR *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			_tcscpy_s( pSlash+1,sizeof(TEXT("DBGHELP.DLL")), TEXT("DBGHELP.DLL") );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}

		*pSlash = _T('\0');
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( TEXT("DBGHELP.DLL"));
	}

	LPCTSTR szResult = NULL;


	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			TCHAR szDumpPath[MAX_PATH];
			TCHAR szScratch [MAX_PATH];

			// work out a good place for the dump file
			//if (!GetTempPath( MAX_PATH, szDumpPath ))
			_tcscpy_s( szDumpPath, szDbgHelpPath);

			SYSTEMTIME sysTime;
			GetLocalTime(&sysTime);

			_stprintf_s( szScratch, TEXT("%d%d%d%d%d%d"), sysTime.wYear, 
														  sysTime.wMonth, 
														  sysTime.wDay,
														  sysTime.wHour,
														  sysTime.wMinute,  
														  sysTime.wSecond);

			_tcscat_s( szDumpPath, TEXT("\\MiniDump_"));
			_tcscat_s( szDumpPath, szScratch);
			_tcscat_s( szDumpPath, TEXT(".dmp"));

			// create the file
			HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, NULL );

			if (hFile!=INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = pException;
				ExInfo.ClientPointers = NULL;

				// write the dump
				BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
				if (bOK)
				{
					_stprintf_s( szScratch, TEXT("Saved dump file to '%s'"), szDumpPath );
					szResult = szScratch;
					retval = EXCEPTION_EXECUTE_HANDLER;
				}
				else
				{
					_stprintf_s( szScratch, TEXT("Failed to save dump file to '%s' (error %d)"), szDumpPath, GetLastError() );
					szResult = szScratch;
				}
				::CloseHandle(hFile);
			}
			else
			{
				_stprintf_s( szScratch, TEXT("Failed to create dump file '%s' (error %d)"), szDumpPath, GetLastError() );
				szResult = szScratch;
			}

		}
		else
		{
			szResult = TEXT("DBGHELP.DLL too old");
		}
	}
	else
	{
		szResult = TEXT("DBGHELP.DLL not found");
	}

	/*if (szResult)
	::MessageBox( NULL, szResult, L"Error", MB_OK );*/

	return retval;
}

unsigned __stdcall CreateMiniDump(void* pArguments)
{
	_MinidumpInfo* pDumpInfo = (_MinidumpInfo*)pArguments;

	ProcessMiniDump(pDumpInfo->pException);

	_endthreadex(0);
	return 0;
}

LONG WINAPI MyMiniDumpHandler(PEXCEPTION_POINTERS pException)
{
	if (EXCEPTION_STACK_OVERFLOW == pException->ExceptionRecord->ExceptionCode)
	{
		MinidumpInfo info;
		info.threadId = ::GetCurrentThreadId();
		info.pException = pException;

		HANDLE hThread = (HANDLE)_beginthreadex(0, NULL, CreateMiniDump, &info, 0, NULL);

		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	else
	{
		ProcessMiniDump(pException);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI MyBugTrapMixHandler(PEXCEPTION_POINTERS pException)
{
	if (EXCEPTION_STACK_OVERFLOW == pException->ExceptionRecord->ExceptionCode)
	{
		MinidumpInfo info;
		info.threadId = ::GetCurrentThreadId();
		info.pException = pException;

		HANDLE hThread = (HANDLE)_beginthreadex(0, NULL, CreateMiniDump, &info, 0, NULL);

		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	else
	{
		if(TRUE == ProcessBugTrapMix(pException))
		{
			RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
			//return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

//////////////////////////////////////////////////////////////
//유틸리티 함수
//////////////////////////////////////////////////////////////
unsigned __stdcall CreateOutofMemoryLog( void* pArguments )
{
	HANDLE hThread = (*(HANDLE*)pArguments);

	CEHStackWalker EHStackWalker;
	EHStackWalker.SetLogFilePath();

	EHStackWalker.ShowCallstack(hThread);
		
	_endthreadex(0);
	return 0;
}

void GetOutofMemoryLogPath(TCHAR* pPathName)
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


	_tcscpy_s( pPathName, MAX_PATH-1, szCurDirectory);
	_tcscat_s( pPathName,  MAX_PATH-1, TEXT("\\OutofMemoryLog.txt"));
}

void GetUserDefinedLogPath(TCHAR* pPathName)
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


	_tcscpy_s( pPathName, MAX_PATH-1, szCurDirectory);
	_tcscat_s( pPathName,  MAX_PATH-1, TEXT("\\UserDefinedLog.txt"));
}

void AddOutofMemoryLogHeader(DWORD dwCurrentThreadId)
{
	wprintf(L"Out of Memory %d\n", dwCurrentThreadId);

	TCHAR szCurrentPath[MAX_PATH] = {0,};
	GetOutofMemoryLogPath(szCurrentPath);

	wofstream outFile(szCurrentPath,ios::app);
	if(!outFile.good())
	{
		return;
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	outFile << "OccurTime " << sysTime.wYear << '/'<< sysTime.wMonth << '/'
		<< sysTime.wDay << ' ' << sysTime.wHour << ':' << sysTime.wMinute 
		<< ':' << sysTime.wSecond << endl;

	outFile << "Out Of Memory. Thread ID : " << dwCurrentThreadId << endl;
}

void InvalidParameterHandler (PCTSTR expression,
							  PCTSTR function,
							  PCTSTR file,
							  unsigned int line,
							  uintptr_t pReserved)
{
	_tprintf(_T("function %s\n"), function);
	_tprintf(_T("File %s Line %d\n"), file, line);
	_tprintf(_T("expression %s\n"), expression);

	RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
}

void PureFunctionCallHandler()
{
	RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
}

BOOL GetLogicalAddress(PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD &offset)
{
	MEMORY_BASIC_INFORMATION mbi;

	if(!VirtualQuery(addr, &mbi, sizeof(mbi))) 
		return FALSE;

	DWORD hMod = (DWORD)mbi.AllocationBase;

	if(!GetModuleFileName((HMODULE)hMod, szModule, len))
		return FALSE;

	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;
	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHdr);

	DWORD rva = (DWORD)addr - hMod; //Relative Virtual Address is Offset from Module Load Address.

	for(unsigned i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++, pSection++)
	{
		DWORD dwSectionStart = pSection->VirtualAddress;
		DWORD dwSectionEnd = dwSectionStart + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

		if (rva >= dwSectionStart && rva <= dwSectionEnd)
		{
			section = i + 1;
			offset = rva - dwSectionStart;
			return TRUE;
		}
	}

	return FALSE;
}

LONG WINAPI ProcessMyUnhandledException(PEXCEPTION_POINTERS pExceptionInfo)
{
	TCHAR szCurrentPath[MAX_PATH] = {0,};
	GetUserDefinedLogPath(szCurrentPath);

	wofstream outFile(szCurrentPath, ios::app);

	if(!outFile.good())
	{
		return NULL;
	}

	TCHAR szStr[2000];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	_stprintf_s(szStr, TEXT("[%d/%d/%d %d:%d:%d] UnHandled Exception Detected!"), sysTime.wYear,
		sysTime.wMonth,
		sysTime.wDay,
		sysTime.wHour,
		sysTime.wMinute,
		sysTime.wSecond);

	outFile << szStr << endl;

	PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

	_stprintf_s(szStr, TEXT("Exception Code : %08X"), pExceptionRecord->ExceptionCode);

	PVOID addr = pExceptionRecord->ExceptionAddress;

	TCHAR szModule[MAX_PATH];
	DWORD dwLength = sizeof(szModule);
	DWORD dwSection = 0;
	DWORD dwOffset = 0;

	MEMORY_BASIC_INFORMATION mbi;

	if(!VirtualQuery(addr, &mbi, sizeof(mbi))) 
		return NULL;

	DWORD hMod = (DWORD)mbi.AllocationBase;

	if(!GetModuleFileName((HMODULE)hMod, szModule, dwLength))
		return NULL;

	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;
	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHdr);

	DWORD rva = 0;
	rva = (DWORD)addr ? (DWORD)addr - hMod : rva;

	for(unsigned i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++, pSection++)
	{
		DWORD dwSectionStart = pSection->VirtualAddress;
		DWORD dwSectionEnd = dwSectionStart + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

		if (rva >= dwSectionStart && rva <= dwSectionEnd)
		{
			dwSection = i + 1;
			dwOffset = rva ? rva - dwSectionStart : dwOffset;
			break;
		}
	}

	_stprintf_s(szStr, TEXT("Fault Address : %04X:%08X %s"), dwSection, dwOffset, szModule);
	outFile << szStr << endl;

	PCONTEXT pCtx = pExceptionInfo->ContextRecord;
	DWORD pc = pCtx->Eip;

	PDWORD pFrame, pPrevFrame;
	pFrame = (PDWORD)pCtx->Ebp;

	do
	{
		TCHAR szModule[MAX_PATH] = _T("");
		DWORD section = 0, offset = 0;

		if(FALSE == GetLogicalAddress((PVOID)pc, szModule, sizeof(szModule), section, offset))
			break;

		_stprintf_s(szStr, L"%08X %08X %04X:%08X %s", pc, pFrame, section, offset, szModule);

		outFile << szStr << endl;

		pc = pFrame[1];
		pPrevFrame = pFrame;
		pFrame = (PDWORD)pFrame[0];

		if((DWORD)pFrame & 3)
			break;

		if(pFrame <= pPrevFrame)
			break;

		if(IsBadWritePtr(pFrame, sizeof(PVOID) * 2))
			break;

	}while(TRUE);

	return NULL;
}

LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{	
	if (EXCEPTION_STACK_OVERFLOW == pExceptionInfo->ExceptionRecord->ExceptionCode)
	{
		MinidumpInfo info;
		info.threadId = ::GetCurrentThreadId();
		info.pException = pExceptionInfo;

		HANDLE hThread = (HANDLE)_beginthreadex(0, NULL, CreateMiniDump, &info, 0, NULL);

		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);		
	}
	else
	{
		return ProcessMyUnhandledException(pExceptionInfo);				
	}

	return EXCEPTION_EXECUTE_HANDLER;
}