// TestEngine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h> 
#include <windows.h> 
#include "SkyMockInterface.h"
#include "I_LuaModule.h"
#include "luatinker.h"
#include "lua.h"
#include "lualib.h"
#include <string>

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef void (*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_LuaModule*(*PGetLuaModule)();


void* GetModuleFunction(HINSTANCE handle, const char* func_name)
{
	return (void*)GetProcAddress(handle, func_name);
}

int cpp_func(int arg1, int arg2)
{
	return arg1 + arg2;
}

extern "C" __declspec(dllimport) void SetSkyMockInterface(SKY_ALLOC_Interface allocInterface,
	SKY_FILE_Interface fileInterface,
	SKY_Print_Interface printInterface);

void show_error(const char* error)
{
	printf("_ALERT -> %s\n", error);
}

void testSample4(lua_State* L);
void testSample5(lua_State* L);
void TestLua56(lua_State* L);
int testLua(lua_State* L);
void TestLua53(lua_State* L);


struct A
{
	A(int v) : value(v) {}
	int value;
};

struct base
{
	base() {}

	const char* is_base() { return "this is base"; }
};

class test : public base
{
public:
	test(int val) : _test(val) {}
	~test() {}

	const char* is_test() { return "this is test"; }

	void ret_void() {}
	int ret_int() { return _test; }
	int ret_mul(int m) const { return _test * m; }
	A get() { return A(_test); }
	void set(A a) { _test = a.value; }

	int _test;
};
test g_test(11);

int main()
{
	HINSTANCE dllHandle = NULL;
	
	//디버그엔진 모듈을 로드한다.
	/*dllHandle = LoadLibrary("Lua5.dll");
	
	//디버그엔진 모듈이 익스포트하는 SetSkyMockInterface 함수와 GetDebugEngineDLL 함수를 얻어낸다.
	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetLuaModule GetLuaModuleInterface = (PGetLuaModule)GetModuleFunction(dllHandle, "GetLuaModule");

	if (!GetLuaModuleInterface || !SetSkyMockInterface)
	{
		printf("GetLuaModuleInterface Aquired failed!\n");
		return 0;
	}

	//SetSkyMockInterface 함수를 사용해서 디버그엔진 모듈에 파일인터페이스와 입출력, 화면출력 인터페이스를 제공한다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
		
	I_LuaModule* pLuaModule = GetLuaModuleInterface();

	if(pLuaModule == nullptr)
	{
		printf("Lua Module Creation Fail!\n");
		return 0;
	}
	char filename[10] = "1.lua";

	pLuaModule->InitLua();
	pLuaModule->DoFile(filename);
	pLuaModule->CloseLua();*/
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);

	// Lua 를 초기화 한다.
	
	for (int i = 1; i < 200; i++)
	{
		lua_State* L = luaL_newstate();

		// Lua 기본 함수들을 로드한다.- print() 사용
		luaopen_base(L);
		luaL_dofile(L, "sample1.lua");
		int result = luaL_dostring(L, "cpp_func(350, 1200)");
		TestLua53(L);

		// 프로그램 종료
		lua_close(L);
	}
	return 0;
}

void testSample4(lua_State* L)
{


	// Lua 테이블을 생성하고 스택에 푸쉬한다.
	luatinker::table haha(L, "haha");

	// haha.value = 1 값을 넣는다.
	haha.set("value", 1);

	// table 내에 table을 만들어 넣는다.
	haha.set("inside", luatinker::table(L));

	// haha.inside 의 포인터를 스택위로 복사한다.
	luatinker::table inside = haha.get<luatinker::table>("inside");

	// inside.value = 2 값을 넣는다.
	inside.set("value", 2);

	// sample4.lua 파일을 로드/실행한다.
	luatinker::dofile(L, "sample4.lua");

	// Lua 에서 생성한 haha.test 값을 읽는다.
	const char* test = haha.get<const char*>("test");
	printf("haha.test = %s\n", test);

	// 전역에 등록하지 않고 Lua 스택에 빈 테이블을 생성한다.(지역변수)
	luatinker::table temp(L);

	// 빈 테이블.name 에 값을 넣는다.
	temp.set("name", "local table !!");

	// table을 의 인자로 사용하여 print_table 을 호출한다.
	luatinker::call<void>(L, "print_table", temp);

	// 함수가 리턴하는 table을 받는다.
	luatinker::table ret = luatinker::call<luatinker::table>(L, "return_table", "give me a table !!");
	printf("ret.name =\t%s\n", ret.get<const char*>("name"));

	
}

void testSample5(lua_State* L)
{



	// lua_State* 가 갖고있는 현재의 스택을 보여준다.
	printf("%s\n", "-------------------------- current stack");
	luatinker::enum_stack(L);

	// 스택에 1을 밀어넣는다.
	lua_pushnumber(L, 1);

	// 현재 스택의 내용을 다시 출력한다.
	printf("%s\n", "-------------------------- stack after push '1'");
	luatinker::enum_stack(L);


	// sample5.lua 파일을 로드/실행한다.
	luatinker::dofile(L, "sample5.lua");

	// test_error() 함수를 호출한다.
	// test_error() 는 실행중 test_error_3() 호출을 시도하다 에러를 발생시킨다.
	// 함수 호출중 발생한 에러는 printf()를 통해서 출력된다.
	printf("%s\n", "-------------------------- calling test_error()");
	luatinker::call<void>(L, "test_error");

	// test_error_3()는 존재하지 않는 함수이다. 호출 자체가 실패한다.
	printf("%s\n", "-------------------------- calling test_error_3()");
	luatinker::call<void>(L, "test_error_3");

	// printf() 대신 유저가 제공하는 에러 출력 루틴을 사용할 수 있다.
	// 이 에러처리 함수는1개의 루아 문자열로 발생한 에러를 전달하게 된다.
	// C++ 에서 등록할 경우 void function(const char*) 형태가 적합하다.
	luatinker::def(L, "_ALERT", show_error);

	luatinker::call<void>(L, "_ALERT", "test !!!");

	// test_error() 함수를 호출한다.
	// 함수 호출중 발생한 에러는 Lua에 등록된 _ALERT()를 통해서 출력된다.
	printf("%s\n", "-------------------------- calling test_error()");
	luatinker::call<void>(L, "test_error");


}

// 함수 형태가 int(*)(lua_State*) 또는 int(*)(lua_State*,T1) 일 경우만 lua_yield() 를 쓸 수 있다.
// 함수 인자가 더 필요할 경우 lua_tinker.h 의 "functor (non-managed)" 코멘트 부분을 참고해서 직접 추가할 것
int TestFunc(lua_State* L)
{
	printf("# TestFunc 실행중\n");
	return lua_yield(L, 0);
}

int TestFunc2(lua_State* L, float a)
{
	printf("# TestFunc2(L,%f) 실행중\n", a);
	return lua_yield(L, 0);
}

class TestClass
{
public:

	// 함수 형태가 int(T::*)(lua_State*) 또는 int(T::*)(lua_State*,T1) 일 경우만 lua_yield() 를 쓸 수 있다.
	// 함수 인자가 더 필요할 경우 lua_tinker.h 의 "class member functor (non-managed)" 코멘트 부분을 참고해서 직접 추가할 것
	int TestFunc(lua_State* L)
	{
		printf("# TestClass::TestFunc 실행중\n");
		return lua_yield(L, 0);
	}

	int TestFunc2(lua_State* L, float a)
	{
		printf("# TestClass::TestFunc2(L,%f) 실행중\n", a);
		return lua_yield(L, 0);
	}
};


lua_State * newLuaThread(lua_State* L);
void TestLua56(lua_State* L)
{

	// Lua 문자열 함수들을 로드한다.- string 사용
	luaopen_string(L);

	// TestFunc 함수를 Lua 에 등록한다.
	luatinker::def(L, "TestFunc", &TestFunc);
	luatinker::def(L, "TestFunc2", &TestFunc2);

	// TestClass 클래스를 Lua 에 추가한다.
	luatinker::class_add<TestClass>(L, "TestClass");
	// TestClass 의 함수를 등록한다.
	luatinker::class_def<TestClass>(L, "TestFunc", &TestClass::TestFunc);
	luatinker::class_def<TestClass>(L, "TestFunc2", &TestClass::TestFunc2);

	// TestClass 를 전역 변수로 선언한다.
	TestClass g_test;
	luatinker::set(L, "g_test", &g_test);

	// sample3.lua 파일을 로드한다.
	luatinker::dofile(L, "sample6.lua");

	// Thread 를 시작한다.
	lua_State *co = lua_newthread(L);
	lua_getglobal(co, "ThreadTest");
	// Thread 를 시작한다.
	int result = 0;
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);

	// Thread 를 다시 시작한다.
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);

	// Thread 를 다시 시작한다.
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);

	// Thread 를 다시 시작한다.
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);
	

	// Thread 를 다시 시작한다.
	printf("* lua_resume() call\n");
	lua_resume(co, L, 0, &result);
	
	
}

lua_State * newLuaThread(lua_State* L)
{
	lua_State * sL = lua_newthread(L); 
	lua_pushvalue(L, -1);
	int nRef = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setmetatable(L, -2);
	lua_getglobal(L, "_G"); 
	lua_setfield(L, -2, "__index");
	lua_setupvalue(L, 1, 1);
	lua_pop(L, 1); return sL;
}


	
	int lua_callback_ = LUA_REFNIL;

	
	LONGLONG get_tickcount() {
		LARGE_INTEGER freq, counter;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&counter);
		return (1000000L * counter.QuadPart / freq.QuadPart) / 1000L;
	}

	std::string load_test_lua() {
		std::string lua_buff;
		FILE *fp = fopen("test.lua", "rb");
		if (fp == NULL) {
			return "";
		}

		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		lua_buff.resize(size);
		fread((void*)lua_buff.data(), size, 1, fp);
		fclose(fp);
		return lua_buff;
	}

	int set_callback(lua_State *L) {
		int ret = -1;
		do {
			int callback = (int)luaL_ref(L, LUA_REGISTRYINDEX); //将栈顶回调放入lua注册表获并取引用
			if (callback == LUA_REFNIL) {
				break;
			}

			lua_callback_ = callback; //保存回调函数的引用
			ret = 0;
		} while (0);
		return ret;
	}

	void register_function(lua_State *L) {
		lua_register(L, "set_callback", set_callback); //注册函数，lua->C
	}

	int reg(lua_State *L) {
		lua_State *co = lua_newthread(L);
		lua_getglobal(co, "reg");
		int result = 0;
		int rs = lua_resume(co, L, 0, &result);
		return rs;
	}

	int call1(lua_State *L) {
		lua_State *co = lua_newthread(L);
		lua_getglobal(co, "lua_callback");
		int result = 0;
		int rs = lua_resume(co, L, 0, &result);
		lua_pop(L, 1); //co留在主线程的栈上，必须pop，否则栈会溢出
		return rs;
	}

	int call2(lua_State *L) {
		lua_State *co = lua_newthread(L);
		lua_rawgeti(co, LUA_REGISTRYINDEX, lua_callback_);
		int result = 0;
		int rs = lua_resume(co, L, 0, &result);
		lua_pop(L, 1); //co留在主线程的栈上，必须pop，否则栈会溢出
		return rs;
	}

	int call3(lua_State *L) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, lua_callback_);
		int rs = lua_pcall(L, 0, 0, 0);
		return rs;
	}

	int call4(lua_State *L) {
		lua_getglobal(L, "lua_callback");
		int rs = lua_pcall(L, 0, 0, 0);
		return rs;
	}

	LONGLONG test_call(int type, int count, lua_State *L) {
		LONGLONG s = get_tickcount();
		switch (type) {
		case 1: {
			for (int i = 0; i < count; i++) {
				call1(L);
			}
			break;
		}
		case 2: {
			for (int i = 0; i < count; i++) {
				call2(L);
			}
			break;
		}
		case 3: {
			for (int i = 0; i < count; i++) {
				call3(L);
			}
			break;
		}
		case 4: {
			for (int i = 0; i < count; i++) {
				call4(L);
			}
			break;
		}
		}

		LONGLONG e = get_tickcount();
		return e - s;
	}

	lua_State *L_ = NULL;
	int testLua(lua_State* L)
	{
		L_ = L;

		luaL_openlibs(L_);
		register_function(L_);
		std::string lua_buffer = load_test_lua();
		int rs = (luaL_loadbuffer(L_, lua_buffer.c_str(), lua_buffer.length(), "test") || lua_pcall(L_, 0, LUA_MULTRET, 0));
		if (rs != LUA_OK) {
			return 0;
		}

		reg(L_);

		int count = 1000000;
		LONGLONG s1 = test_call(1, count, L_);
		LONGLONG s2 = test_call(2, count, L_);
		LONGLONG s3 = test_call(3, count, L_);
		LONGLONG s4 = test_call(4, count, L_);
		printf("coroutine+no register callback=%I64dms\n", s1);
		printf("coroutine+register callback=%I64dms\n", s2);
		printf("no coroutine+register callback=%I64dms\n", s3);
		printf("no coroutine+no register callback=%I64dms\n", s4);
		getchar();
		return 0;
	}



	void TestLua53(lua_State* L)
	{
		

		// Lua 문자열 함수들을 로드한다.- string 사용
		luaopen_string(L);

		// base 클래스를 Lua 에 추가한다.
		luatinker::class_add<base>(L, "base");
		// base 의 함수를 등록한다.
		luatinker::class_def<base>(L, "is_base", &base::is_base);

		// test 클래스를 Lua 에 추가한다.
		luatinker::class_add<test>(L, "test");
		// test 가 base 에 상속 받았음을 알려준다.
		luatinker::class_inh<test, base>(L);

		// test 클래스 생성자를 등록한다.
		luatinker::class_con<test>(L, luatinker::constructor<test, int>);
		// test 의 함수들을 등록한다.
		luatinker::class_def<test>(L, "is_test", &test::is_test);
		luatinker::class_def<test>(L, "ret_void", &test::ret_void);
		luatinker::class_def<test>(L, "ret_int", &test::ret_int);
		luatinker::class_def<test>(L, "ret_mul", &test::ret_mul);
		luatinker::class_def<test>(L, "get", &test::get);
		luatinker::class_def<test>(L, "set", &test::set);
		luatinker::class_mem<test>(L, "_test", &test::_test);

		// Lua 전역 변수호 g_test 의 포인터를 등록한다.
		luatinker::set(L, "g_test", &g_test);

		// sample3.lua 파일을 로드/실행한다.
		luatinker::dofile(L, "sample3.lua");
	}


