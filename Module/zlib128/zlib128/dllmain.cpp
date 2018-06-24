// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.

#include "windef.h"
#include "SkyMockInterface.h"
#include "SkyInterface.h"
#include "zlib.h"
#include "string.h"
#include "stl/IOStream.h"
#include "sprintf.h"
#include "ZlibCompress.h"

extern FILE* g_skyStdErr;
extern FILE* g_skyStdOut;

void printf(const char* str, ...)
{
	if (!str)
		return;

	va_list		args;
	va_start(args, str);
	size_t i;
	for (i = 0; i < strlen(str); i++) {

		switch (str[i]) {

		case '%':

			switch (str[i + 1]) {

				/*** characters ***/
			case 'c': {
				char c = va_arg(args, char);
				g_mockInterface.g_printInterface.sky_printf("%c", c);
				//SkyConsole::WriteChar(c);
				i++;		// go to next character
				break;
			}

					  /*** address of ***/
			case 's': {
				int c = (int&)va_arg(args, char);
				char str[256];
				strcpy(str, (const char*)c);
				g_mockInterface.g_printInterface.sky_printf("%s",str);
				//SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

					  /*** integers ***/
			case 'd':
			case 'i': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 10, str);
				g_mockInterface.g_printInterface.sky_printf("%s", str);
				//SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

					  /*** display in hex ***/
					  /*int*/
			case 'X': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				//SkyConsole::Write(str);
				g_mockInterface.g_printInterface.sky_printf("%s", str);
				i++;		// go to next character
				break;
			}
					  /*unsigned int*/
			case 'x': {
				unsigned int c = va_arg(args, unsigned int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				//SkyConsole::Write(str);
				g_mockInterface.g_printInterface.sky_printf("%s", str);
				i++;		// go to next character
				break;
			}

			default:
				va_end(args);
				return;
			}

			break;

		default:
//			SkyConsole::WriteChar(str[i]);
			g_mockInterface.g_printInterface.sky_printf("%c", str[i]);
			break;
		}

	}

	va_end(args);
	return;
}
#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0
#define DLL_THREAD_ATTACH 2
#define DLL_THREAD_DETACH 3

extern void _cdecl InitializeConstructors();
extern void _cdecl Exit();

bool __stdcall DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	//gzFile file;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		
		

	}
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


extern "C" bool WINAPI _DllMainCRTStartup(
	HANDLE  hDllHandle,
	DWORD   dwReason,
	LPVOID  lpreserved
)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// set up our minimal cheezy atexit table
		//_atexit_init();

		// Call C++ constructors
		//_initterm(__xc_a, __xc_z);

		InitializeConstructors();
	}

	bool retcode = DllMain(hDllHandle, dwReason, lpreserved);

	if (dwReason == DLL_PROCESS_DETACH)
	{
		Exit();
	}

	return retcode;
}


void TestZLib()
{
	const  int BUF = 1024;
	const  int DBUF = BUF * 2 + 13;

	Bytef raw_data[] = "안녕하세요.";
	Bytef deflate_data[DBUF];

	uLong raw_size = strlen((const  char*)raw_data);
	uLong deflate_size = DBUF;

	//compress 사용하기
	{
		compress(deflate_data, &deflate_size, raw_data, raw_size);
		std::cout << "Raw Data Size:" << raw_size << std::endl;
		std::cout << "Deflate Data Size:" << deflate_size << std::endl;
	}

	Bytef inflate_data[BUF];
	uLong inflate_size = BUF;
	//uncompress 사용하기
	{
		uncompress(inflate_data, &inflate_size, deflate_data, deflate_size);
		std::cout << "Deflate Data Size:" << deflate_size << std::endl;
		std::cout << "Inflate Size:" << inflate_size << std::endl;
		inflate_data[inflate_size] = NULL;
		std::cout << "원본  데이터:" << (const  char*)inflate_data << std::endl;
	}
}


extern "C" __declspec(dllexport) I_Compress* GetZlibCompress()
{
	return new ZlibCompress();
}