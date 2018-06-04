#include "SkyTest.h"
#include "SkyConsole.h"
#include "MultiBoot.h"

//인터럽트 핸들러 테스트
void TestDivideByZero();
void TestBreakPoint();
void TestInvalidOpcode();

void TestInterrupt()
{
	TestDivideByZero();
	//TestBreakPoint();
	//TestInvalidOpcode();
}

int _divider = 0;
int _dividend = 100;
void TestDivideByZero()
{	
	int result = _dividend / _divider;

//예외처리를 통해 아래 코드가 실행된다고 해도
//result 결과는 정상적인 값이 아니다
//위의 한줄은 어셈블리 명령어단에서 보면 여러 줄이며 
//중간정도에서 오동작 부분을 수정했다 해서 정상적인 결과를 기대하는 것은 무리다.

	if(_divider != 0)
		result = _dividend / _divider;

	SkyConsole::Print("Result is %d, divider : %d\n", result, _divider);
}

void funcBreakPoint(void)
{
	__asm {
		align 4		
		__asm _emit 0xcc
		__asm _emit 0x00
		__asm _emit 0x00
		__asm _emit 0x00
	}
}

void TestBreakPoint()
{
	funcBreakPoint();
}

void TestInvalidOpcode()
{
	__asm {
		align 4
		__asm _emit 0xFF
		__asm _emit 0xFF
		__asm _emit 0xFF
		__asm _emit 0xFF
	}
}
//인터럽트 핸들러 테스트 끝




