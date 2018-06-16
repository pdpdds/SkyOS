#include "SkyOS.h"

static void* _syscalls[] = {

	kprintf,
	TerminateProcess,
	MemoryAlloc,
	MemoryFree,
	CreateDefaultHeap,
	GetTickCount,
	CreateThread,
	ksleep,
};

_declspec(naked) void SysCallDispatcher()
{

	//서비스 API 번호를 IDX에 저장하고 데이터 셀렉터를 변경한다(0X10)
	static uint32_t idx = 0;
	_asm {
		push eax
		mov eax, 0x10
		mov ds, ax
		pop eax
		mov[idx], eax
		pusha
	}


	// 요청한 서비스 API의 인덱스가 최대값보다 크면 아무런 처리를 하지 않는다.
	if (idx >= MAX_SYSCALL) {
		_asm {
			/* restore registers and return */
			popa
			iretd
		}
	}

	//서비스 인덱스 번호에 해당하는 시스템 함수를 얻어낸다.
	static void* fnct = 0;
	fnct = _syscalls[idx];

	//시스템 함수를 실행한다.
	_asm {
		//레지스터들을 복원한 후 함수의 파라메터로 집어넣는다.
		popa
		push edi
		push esi
		push edx
		push ecx
		push ebx
		//시스템 콜
		call fnct
		//스택 정리 책임은 콜러에게 있다. 파라메터로 5개를 스택에 집어넣었으므로
		//스택 포인터의 값을 20바이트 증가시킨다.
		add esp, 20
		//커널 데이터 셀렉터값을 유저 데이터 셀렉터값으로 변경한다.	
		push eax
		mov eax, 0x23
		mov ds, ax
		pop eax
		iretd
	}
}

void InitializeSysCall()
{
	SetInterruptVector(0x80, SysCallDispatcher, I86_IDT_DESC_RING3);
}
