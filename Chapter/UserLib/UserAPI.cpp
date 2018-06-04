#include "UserAPI.h"
#include "va_list.h"
#include "stdarg.h"
#include "sprintf.h"
#include "string.h"
#include "stl\stl_string.h"


int kprintf(const char* szMsg);

void printf(const char* str, ...)
{
	if (!str)
		return;
	
	std::string buffer;

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
				buffer += c;				
				i++;		// go to next character
				break;
			}

					  /*** address of ***/
			case 's': {
				int c = (int&)va_arg(args, char);
				char str[256];
				strcpy(str, (const char*)c);
				buffer += str;				
				i++;		// go to next character
				break;
			}

					  /*** integers ***/
			case 'd':
			case 'i': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 10, str);
				buffer += str;
				i++;		// go to next character
				break;
			}

					  /*** display in hex ***/
					  /*int*/
			case 'X': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				buffer += str;
				i++;		// go to next character
				break;
			}
					  /*unsigned int*/
			case 'x': {
				unsigned int c = va_arg(args, unsigned int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				buffer += str;
				i++;		// go to next character
				break;
			}
			case 'f':
				double double_temp;
				double_temp = va_arg(args, double);
				char str[512];
				ftoa_fixed(str, double_temp);
				buffer += str;
				i++;
				break;

		

			default:
				va_end(args);
				return;
			}

			break;

		default:			
			buffer += str[i];
			break;
		}

	}

	va_end(args);

	
	kprintf(buffer.c_str());
	return;
}


//커널 콘솔 화면에 문자열 출력
int kprintf(const char* szMsg)
{
	__asm
	{
		mov ebx, szMsg
		mov eax, 0
		int 0x80
	}
}

//프로세스 종료
void TerminateProcess()
{
	__asm
	{
		mov eax, 1
		int 0x80
	}
}

u32int malloc(u32int sz)
{
	int address = 0;
	__asm {
		mov ebx, sz
		mov eax, 2
		int 0x80
		mov address, eax
	}

	return address;
}

void free(void *p)
{
	__asm {

		mov ebx, p
		mov eax, 3
		int 0x80
	}
}


//힙을 생성
void CreateHeap()
{
	__asm {

		mov eax, 4
		int 0x80
	}
}

//현재의 틱을 얻어온다.
uint32_t GetTickCount()
{
	int address = 0;
	__asm
	{
		mov eax, 5
		int 0x80
		mov address, eax
	}
	return address;
}

char GetKeyboardInput()
{
	__asm {

		mov eax, 6
		int 0x80
	}
}

void sleep(int millisecond)
{
	__asm {

		mov ebx, millisecond
		mov eax, 7
		int 0x80
	}
}

void *operator new(size_t size)
{
	return (void *)malloc(size);
}

void *operator new[](size_t size)
{
	return (void *)malloc(size);
}

void operator delete(void *p)
{
	free(p);
}

void operator delete(void *p, size_t size)
{
	free(p);
}

//순수 가상 함수 호출 에러에 대해서는 아무런 처리를 하지 않는다.
int __cdecl _purecall()
{
	return 0;
}

void operator delete[](void *p)
{
	free(p);
}