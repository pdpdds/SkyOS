#include "SkyTest.h"
#include "SkyConsole.h"
#include "MultiBoot.h"
#include "TestInteger.h"
#include "PureCallExtend.h"
#include "ZetPlane.h"
#include "string.h"
#include "memory.h"
#include "kheap.h"
#include "skystruct.h"
#include "jsmn.h"

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

	if (_divider != 0)
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
void TestCPP14();
void TestPureFunctionCall();
void TestHeapCorruption();
void TestHeapCorruption2();
void TestInheritance();

void TestCPlusPlus()
{
	TestCPP14();
	//TestPureFunctionCall();
	//TestHeapCorruption();
	//TestHeapCorruption2();
	//TestInheritance();
}

void TestCPP14()
{
	SkyConsole::Print("\nCPP14 Test\n\n");

	auto func = [x = 5]() { return x; };
	auto n1 = func();
	SkyConsole::Print("Lamda n1:%d\n", n1);

	constexpr TestInteger size(10);
	int x[size];
	x[3] = 11;
	SkyConsole::Print("constexor x[3]:%d\n", x[3]);
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
//C++ 테스트 끝

//공통 라이브러리 테스트
void TestMap();
void Testlist();
void TestString();
void TestVector();
void TestStack();
void TestDeque();
void TestQueue();

using namespace std;

void TestCommonLibrary()
{
	TestString();
	//TestMap();
	//Testlist();	
	//TestVector();
	//TestStack();
	//TestDeque();
	//TestQueue();
}

void TestQueue()
{
	queue queue;
	QueueNode* pNode = new QueueNode();
	pNode->_data = (void*)5;
	queue.Enqueue(pNode);

	QueueNode* pNode2 = new QueueNode();
	pNode2->_data = (void*)10;
	queue.Enqueue(pNode2);

	QueueNode* result = queue.Dequeue();
	delete result;
	result = queue.Dequeue();
	delete result;
}

void TestStack()
{
	stack<int> s;
	s.push(10);
	s.push(20);
	s.push(30);

	while (s.size() > 0)
	{
		int data = s.pop();
		SkyConsole::Print("%d\n", data);
	}
}

void TestVector()
{
	vector<int> vec;
	vec.push_back(5);
	vec.push_back(1);
	vec.push_back(3);
	vec.push_back(4);

	while (vec.size() > 0)
	{
		int data = vec.back();
		SkyConsole::Print("%d\n", data);
		vec.pop_back();
	}

	for (;;);
}

void TestMap()
{
	map<int, int> hashmap;
	map<int, int> m10;
	map<int, int>::iterator it1;
	map<int, int>::iterator it2;

	m10[1] = 2;
	m10[2] = 4;
	m10[4] = 8;
	m10[3] = 6;
	m10[5] = 10;
	m10[6] = 12;
	m10[7] = 14;
	m10[8] = 16;

	int i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		SkyConsole::Print("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}

	SkyConsole::Print("\n\n");

	//cout << "ERASE BY KEY" << endl;
	m10.erase(3);
	i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		SkyConsole::Print("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}
}

typedef struct tag_testStruct
{
	int j;
	int k;
}testStruct;

void Testlist()
{
	list<testStruct> fstlist;
	list<int> scndlist;
	int counter = 0;

	for (int i = 0; i <= 10; ++i) {
		testStruct a;
		a.j = i;
		a.k = i + 1;
		fstlist.push_back(a);
	}
	list<testStruct>::iterator iter = fstlist.begin();
	for (size_t i = 0; i < fstlist.size(); i++, iter++)
	{
		SkyConsole::Print("item 0x%d 0x%d done\n", ((testStruct)(*iter)).j, ((testStruct(*iter)).k));
	}

	SkyConsole::Print("done!!\n");
}



void TestString()
{
	std::string str("abc");
	SkyConsole::Print("%s\n", str.c_str());

	std::string str2 = str;
	str2 = "cdf";
	SkyConsole::Print("%s\n", str2.c_str());
}

void TestDeque()
{
	std::deque<int> a;

	for (int i = 1; i < 6; i++)
		a.push_front(i);

	for (int i = 0; i < 5; i++)
		SkyConsole::Print("%d\n", a[i]);
}

//공통 라이브러리 테스트 끝