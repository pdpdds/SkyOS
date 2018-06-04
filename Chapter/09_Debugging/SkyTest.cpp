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
#include "HDDAdaptor.h"
#include "StorageManager.h"
#include "fileio.h"
#include "ctrycatch.h"

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
void TestJson();
void TestString();
void TestVector();
void TestStack();
void TestDeque();
void TestQueue();

using namespace std;

void TestCommonLibrary()
{

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

static const char *JSON_STRING =
"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void TestJson()
{
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		SkyConsole::Print("Failed to parse JSON: %d\n", r);
		return;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		SkyConsole::Print("Object expected\n");
		return;
	}

	char buf[256];
	memset(buf, 0, 256);


	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "user") == 0) {
			/* We may use strndup() to fetch string value */
			memcpy(buf, JSON_STRING + t[i + 1].start, t[i + 1].end - t[i + 1].start);
			SkyConsole::Print("- User: %s\n", buf);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {

			SkyConsole::Print("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {

			SkyConsole::Print("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) {
			int j;
			SkyConsole::Print("- Groups:\n");
			if (t[i + 1].type != JSMN_ARRAY) {
				continue;
			}
			for (j = 0; j < t[i + 1].size; j++) {
				jsmntok_t *g = &t[i + j + 2];
				SkyConsole::Print("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i + 1].size + 1;
		}
		else {
			SkyConsole::Print("Unexpected key: %.*s\n", t[i].end - t[i].start,
				JSON_STRING + t[i].start);
		}
	}
}

/*void TestMap()
{
	std::map<int, int> m10;
	std::map<int, int>::iterator it1;
	std::map<int, int>::iterator it2;

	m10[1] = 2;
	m10[2] = 4;
	m10[3] = 6;
	m10[4] = 8;
	m10[5] = 10;
	m10[6] = 12;
	m10[7] = 14;
	m10[8] = 16;
	m10[8] = 18;
	int i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		SkyConsole::Print("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}

	//cout << "ERASE BY KEY" << endl;
	m10.erase(3);
	i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		SkyConsole::Print("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}
}*/


void TestString()
{
	std::string str("abc");
	SkyConsole::Print("%s\n", str.c_str());

	std::string str2 = str;
	str2 = "cdf";
	SkyConsole::Print("%s\n", str2.c_str());

	/*void test(String str)
	{
	cout << str << endl;
	}

	int main()
	{
	String bar = "Hello";
	cout << ("a" + String("b")) << endl;
	}*/

	/*int main()
	{
	String a = "foo";
	String b = "bar";
	a = b;
	cout << a << endl;
	}*/

	/*int main()
	{
	cout << "|" << String("  baz  ").trim() << "|" << endl;
	cout << "|" << String("  baz  ").trimleft() << "|" << endl;
	cout << "|" << String("  baz  ").trimright() << "|" << endl;
	cout << "|" << String("  ").trim() << "|" << endl;
	cout << "|" << String("b  ").trim() << "|" << endl;
	cout << "|" << String(" b  ").trim() << "|" << endl;
	cout << "|" << String("  b").trim() << "|" << endl;
	cout << "|" << String("  b ").trim() << "|" << endl;
	}

	int main()
	{
	String str;

	int count = str.getline(cin);

	cout << str << endl;
	cout << count << endl;
	}*/

	/*void test(const String &a, String b)
	{
	cout << a << b << endl;
	}

	int main()
	{
	String a = "foo";
	cout << a << endl;
	cout << "---" << endl;

	cout << strlen(a) << " should equal " << a.length() << endl;
	cout << a + "bar" << endl;
	test("foo", "bar");
	cout << "---" << endl;

	cout << a << endl;

	cout << a.substr(0, 2) << endl;
	cout << a.substr(1, 2) << endl;
	cout << a.substr(0, 4) << endl;
	cout << a.substr(3) << endl;
	cout << a.substr(3, 3) << endl;
	cout << a.substr(-2) << endl;
	cout << a.substr(-2, 2) << endl;
	cout << a.substr(-3) << endl;
	cout << a.substr(-3, 2) << endl;
	cout << a.substr(-3, 3) << endl;
	cout << a.substr(-4) << endl;
	cout << a.substr(-4, 4) << endl;
	cout << a.substr(-4, 5) << endl;
	cout << "---" << endl;

	cout << a[0] << endl;
	cout << a.get(0) << endl;
	cout << a[-3] << endl;
	cout << a.get(-3) << endl;

	a.set(0, 'F');
	cout << a << endl;
	a.set(-3, 'B');
	cout << a << endl;

	cout << a.set(4, 'A') << endl;
	cout << a.set(-4, 'A') << endl;
	cout << a.set(0, '\0') << endl;
	cout << a << endl;
	cout << "---" << endl;

	String c = "123";
	cout << (atoi(c) + 1) << endl;
	cout << "---" << endl;

	String d;
	cout << "Enter some text and then press enter:" << endl;
	cout << "Entered " << d.getline(cin) << " characters." << endl;
	cout << d << endl;

	return 0;
	}*/
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


//하드디스크 테스트
void TestHardDisk()
{
	FILE* pFile = fopen("MENU.LST", "rw");

	if (pFile != NULL)
	{
		SkyConsole::Print("Handle ID %d\n", pFile->_id);

		BYTE* buffer = new BYTE[512];
		int ret = StorageManager::GetInstance()->ReadFile(pFile, buffer, 512, 1);

		if (ret > 0)
			SkyConsole::Print("%s\n", buffer);

		StorageManager::GetInstance()->CloseFile(pFile);		

		delete buffer;
	}
}

void TestFPU()
{
	float sampleFloat = 0.3f;

	sampleFloat *= 5.482f;

	SkyConsole::Print("sample Float Value %f\n", sampleFloat);
}

//Try Catch 테스트

void throwArgumentException() {
	//puts("Function reached.");
	throw(ArgumentException, (char*)"Ooh! Some ArgumentException was thrown. ");
}

void TestTryCatch()
{
	try {
		throwArgumentException();
	}
	catch (ArgumentException) {
		//	puts("ArgumentException block reached");
		if (__ctrycatch_exception_message_exists)
			SkyConsole::Print("message: %s\n", __ctrycatch_exception_message);
	}
	finally {
		//puts("finally block reached");
		SkyConsole::Print("Finally!!\n");
	}
}

void TestNullPointer()
{
	ZetPlane* pPlane = 0;
	pPlane->IsRotate();
}

void TestDebugging()
{
	double fCrashVar = 1.3f;
	//TestTryCatch();
	//TestNullPointer();
}