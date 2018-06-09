#pragma once
#include "I_LuaModule.h"

class LuaModule : public I_LuaModule
{
public:
	LuaModule();
	~LuaModule();

	virtual bool InitLua() override;
	virtual bool DoFile(char* fileName) override;
	virtual bool CloseLua() override;
	virtual bool AddFunc(char* funcName, void* func) override;
	virtual bool CallFunc(char* funcName, const char *format, ...) override;
};

