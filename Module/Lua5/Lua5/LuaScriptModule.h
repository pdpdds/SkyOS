#pragma once
#include "I_LuaModule.h"

class LuaScript;

class LuaScriptModule : public I_LuaModule
{
public:
	LuaScriptModule();
	~LuaScriptModule();

	virtual bool InitLua() override;
	virtual bool DoFile(char* fileName) override;
	virtual bool CloseLua() override;

	void GetElement(float& value, char* elementName);
	void GetElement(char* buffer, char* elementName);
	void GetElement(int& value, char* elementName);

	int GetIntArray(int* pArray, char* elementName);

private:
	LuaScript* m_pScript;
};


