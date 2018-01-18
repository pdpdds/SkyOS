#include "UserAPI.h"

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

void free(void *p)
{
	__asm {

		mov ebx, p
			mov eax, 3
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

//힙을 생성
void CreateHeap()
{
	__asm {

		mov eax, 4
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

//커널 콘솔 화면에 문자열 출력
int printf(const char* szMsg)
{
	__asm 
	{		
		mov ebx, szMsg
		mov eax, 0
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