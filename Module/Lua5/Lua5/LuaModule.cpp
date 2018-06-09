#include "LuaModule.h"
#include "lua.h"
#include "luascript.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "SkyMockInterface.h"
#include "luatinker.h"

lua_State* g_L;

LuaModule::LuaModule()
{
}


LuaModule::~LuaModule()
{
}
int cpp_func(int arg1, int arg2)
{
	return arg1 + arg2;
}

bool LuaModule::InitLua()
{
	// Lua 를 초기화 한다.
	g_L = luaL_newstate();

	// Lua 기본 함수들을 로드한다.- print() 사용
	return luaopen_base(g_L) == 1;
}

bool LuaModule::AddFunc(char* funcName, void* func)
{
	if (g_L == nullptr)
		return false;

	// LuaTinker 를 이용해서 함수를 등록한다.
	//luatinker::def(g_L, funcName, func);

	return true;
}

bool LuaModule::DoFile(char* fileName)
{
	if (g_L == nullptr)
		return false;

	//luatinker::dofile(g_L, fileName);

	return true;
}

bool LuaModule::CloseLua()
{
	// 프로그램 종료
	lua_close(g_L);
	g_L = nullptr;

	return true;
}

bool LuaModule::CallFunc(char* funcName, const char *format, ...)
{
	// sample1.lua 의 함수를 호출한다.
	//int result = luatinker::call<int>(L, "lua_func", 3, 4);

	// lua_func(3,4) 의 결과물 출력
	//printf("lua_func(3,4) = %d\n", result);

	return true;
}


/*LuaScript* script = new LuaScript("Player.lua");
float posX = script->get<float>("player.position.x");
float posY = script->get<float>("player.position.y");
std::string filename = script->get<std::string>("player.filename");
int hp = script->get<int>("player.HP");

//std::cout << "Position X = " << posX << ", Y = " << posY << std::endl;
//std::cout << "Filename:" << filename << std::endl;
//std::cout << "HP:" << hp << std::endl;

// getting arrays
std::vector<int> v = script->getIntVector("array");
//std::cout << "Contents of array:";
for (std::vector<int>::Iterator it = v.begin();
it != v.end();
it++) {
g_mockInterface.g_printInterface.sky_printf("%d\n", *it);
//	std::cout << *it << ",";
}
//std::cout << std::endl;

// getting table keys:
std::vector<std::string> keys = script->getTableKeys("player");
//std::cout << "Keys of [player] table:";
for (std::vector<std::string>::Iterator it = keys.begin();
it != keys.end();
it++) {
g_mockInterface.g_printInterface.sky_printf("%s\n", *it);
//std::cout << *it << ",";
}
//std::cout << std::endl;*/