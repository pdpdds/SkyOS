#include "LuaModule.h"
#include "lua.h"
#include "lualib.h"

LuaModule::LuaModule()
{
}


LuaModule::~LuaModule()
{
}

bool LuaModule::InitLua()
{
	lua_open();
	lua_pushstring("> ");
	lua_setglobal("_PROMPT");
	lua_userinit();
	return true;
}

bool LuaModule::DoFile(char* fileName)
{
	int result = lua_dofile(fileName);

	return result == 0;
}

bool LuaModule::CloseLua()
{
	lua_close();
	return true;
}
