#include "SkyOS.h"
#include "TestInteger.h"
#include "PureCallExtend.h"
#include "ZetPlane.h"
#include "jsmn.h"
#include "ctrycatch.h"
#include "I_Compress.h"
//#include "hash_map.h"

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
	jsmn_parser p; //파서
	jsmntok_t t[128]; //토큰 개수는 128개로 제한

	//파서를 초기화하고 스트링을 파싱한다.
	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		SkyConsole::Print("Failed to parse JSON: %d\n", r);
		return;
	}

	// 최상위 요소는 오브젝트라 가정한다.
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		SkyConsole::Print("Object expected\n");
		return;
	}

	char buf[256];
	memset(buf, 0, 256);

	//각각의 루트 오브젝트에 대해 루프를 돈다.
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "user") == 0) 
		{
			//user의 value값을 얻는다.
			memcpy(buf, JSON_STRING + t[i + 1].start, t[i + 1].end - t[i + 1].start);
			SkyConsole::Print("- User: %s\n", buf);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "admin") == 0)
		{
			//admin의 value값을 얻는다.
			SkyConsole::Print("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "uid") == 0)
		{
			//uid의 value값을 얻는다.
			SkyConsole::Print("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) 
		{
			int j;
			//groups 키는 배열값을 가진다.
			SkyConsole::Print("- Groups:\n");
			if (t[i + 1].type != JSMN_ARRAY) {
				continue;
			}
			//배열을 파싱해서 토큰을 얻어낸다.
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

	//cout << "ERASE BY KEY" << endl;
	m10.erase(3);
	i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		SkyConsole::Print("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}
}

void TestString()
{
	std::string str("abc");
	SkyConsole::Print("%s\n", str.c_str());

	std::string str2 = str;
	str2 = "cdf";
	SkyConsole::Print("%s\n", str2.c_str());

	std::string a = "foo";
	std::string b = "bar";
	a = b;
	cout << a.c_str() << endl;	
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

#include "lua.h"
//#include "luadebug.h"
#include "lualib.h"

//저장장치 테스트
void TestStorage(const char* filename, char driveLetter)
{
	StorageManager::GetInstance()->SetCurrentFileSystemByID(driveLetter);

	FILE* pFile = fopen(filename, "r");
	
	if (pFile != NULL)
	{
		SkyConsole::Print("Handle ID : %d\n", pFile->_id);

		BYTE* buffer = new BYTE[512];
		memset(buffer, 0, 512);
		int ret = fread(buffer, 511, 1, pFile);

		if (ret > 0)
			SkyConsole::Print("%s [%d]\n", buffer, ret);

		fclose(pFile);		

		delete buffer;
	}

	SkyConsole::Print("Test End\n");
	for (;;);
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

typedef void(*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_Compress* (*PGetEasyCompress)();
extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

char easyTestBuffer[] = "Sky OS Compression Test!!";
void TestEasyZLib()
{	
	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');

	MODULE_HANDLE hwnd = SkyModuleManager::GetInstance()->LoadModuleFromMemory("zlib.dll");
	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "SetSkyMockInterface");	
	PGetEasyCompress GetEasyCompress = (PGetEasyCompress)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetEasyCompress");
	//디버그 엔진에 플랫폼 종속적인 인터페이스를 넘긴다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);

	if (!GetEasyCompress)
	{
		HaltSystem("HanguleMint64Engine Module Load Fail!!");
	}

	I_Compress* pEasyCompress = GetEasyCompress();

	char* destBuffer = new char[256];
	long destBufferLen = 256;
	long nSrcLen = sizeof(easyTestBuffer);

	char* decompressedBuffer = new char[256];
	long decompressedLen = 256;

	memset(destBuffer, 0, 256);
	memset(decompressedBuffer, 0, 256);

	SkyConsole::Print("text : %s\n", easyTestBuffer);
	
	//압축한다.
	if (0 != pEasyCompress->Compress((unsigned char* )destBuffer, &destBufferLen, ( unsigned char* )easyTestBuffer, nSrcLen))
	{
		HaltSystem("easyzlib test fail!!");
	}
	SkyConsole::Print("Compressed : Src Size %d, Dest Size %d\n", nSrcLen, destBufferLen);

	//압축을 해제한다. 
	if (0 != pEasyCompress->Decompress((unsigned char*)decompressedBuffer, &decompressedLen, ( unsigned char*)destBuffer, destBufferLen))
	{
		HaltSystem("easyzlib test fail!!");
	}
	SkyConsole::Print("Decompressed : Src Size %d, Dest Size %d\n", destBufferLen, decompressedLen);
	SkyConsole::Print("result : %s\n", decompressedBuffer);

	delete destBuffer;
	delete decompressedBuffer;
}

bool TestMemoryModule(const char* moduleName)
{
	MODULE_HANDLE hwnd = SkyModuleManager::GetInstance()->LoadModuleFromMemory(moduleName);

	if (hwnd == nullptr)
	{
		HaltSystem("Memory Module Load Fail!!");
	}

	PGetDLLInterface GetDLLInterface = (PGetDLLInterface)SkyModuleManager::GetInstance()->GetModuleFunction((MODULE_HANDLE)(hwnd), "GetDLLInterface");

	if (!GetDLLInterface)
	{
		HaltSystem("Memory Module Load Fail!!");
	}

	const DLLInterface* dll_interface = GetDLLInterface();

	int sum = dll_interface->AddNumbers(5, 6);

	SkyConsole::Print("AddNumbers(5, 6): %d\n", sum);

	if (false == SkyModuleManager::GetInstance()->UnloadModule((MODULE_HANDLE)(hwnd)))
		HaltSystem("UnloadDLL() failed!\n");

	return true;
}
