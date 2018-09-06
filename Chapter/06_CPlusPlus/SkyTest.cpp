#include "SkyTest.h"
#include "SkyConsole.h"
#include "MultiBoot.h"
#include "TestInteger.h"
#include "PureCallExtend.h"
#include "ZetPlane.h"
#include "string.h"
#include "memory.h"
#include "kheap.h"

//인터럽트 핸들러 테스트
void TestDivideByZero();
void TestBreakPoint();
void TestInvalidOpcode();

void TestInterrupt()
{
	//TestDivideByZero();
	//TestBreakPoint();
	TestInvalidOpcode();
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


//C++ 테스트 
void TestCPP11();
void TestPureFunctionCall();
void TestHeapCorruption();
void TestHeapCorruption2();
void TestInheritance();

void TestCPlusPlus()
{
	TestCPP11();
	//TestPureFunctionCall();
	//TestHeapCorruption();
	//TestHeapCorruption2();
	//TestInheritance();
}

void TestCPP11()
{
	SkyConsole::Print("\nCPP11 Test\n\n");

	auto func = [x = 5]() { return x; };
	auto n1 = func();
	SkyConsole::Print("Lamda n1:%d\n", n1);

	constexpr TestInteger size(10);
	int x[size];
	x[3] = 11;
	SkyConsole::Print("constexpr x[3]:%d\n", x[3]);
}

void TestPureFunctionCall()
{
	SkyConsole::Print("\nPure Function Call Test\n\n");
	PureCallExtend pureCall;
}

void TestHeapCorruption()
{
	SkyConsole::Print("\nHeap Corruption Test\n\n");
	Plane* pPlane = new Plane();

	//블록의 푸터 조작
	*((char*)pPlane + sizeof(Plane) + 1) = (char)0;

	delete pPlane;
}

void TestHeapCorruption2()
{
	SkyConsole::Print("\nHeap Corruption Test2\n\n");
	Plane* pPlane = new Plane();

	//가상함수 테이블 망가뜨리기
	memset(pPlane, 0, sizeof(Plane));
	
	delete pPlane;
}

void TestInheritance()
{
	SkyConsole::Print("\nClass Inheritance Test\n\n");
	
	int i = 100;
	Plane* pPlane = new ZetPlane();
	pPlane->SetX(i);
	pPlane->SetY(i + 5);

	pPlane->IsRotate();

	SkyConsole::Print("Plane X : %d, Plane Y : %d\n", pPlane->GetX(), pPlane->GetY());

	delete pPlane;
}