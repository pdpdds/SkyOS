#pragma once

class I_LuaModule
{
public:
	virtual bool InitLua() = 0;
	virtual bool DoFile(char* fileName) = 0;
	virtual bool CloseLua() = 0;
	virtual bool AddFunc(char* funcName, void* func) { return false; }
	virtual bool CallFunc(char* funcName, const char *format, ...) { return false; }
};