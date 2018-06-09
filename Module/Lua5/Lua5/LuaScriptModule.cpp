#include "LuaScriptModule.h"
#include "lua.h"
#include "luascript.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "SkyMockInterface.h"
#include "luatinker.h"

LuaScriptModule::LuaScriptModule()
{
	m_pScript = nullptr;
}


LuaScriptModule::~LuaScriptModule()
{
}

bool LuaScriptModule::InitLua()
{
	return true;
}

bool LuaScriptModule::CloseLua()
{

	return true;
}

bool LuaScriptModule::DoFile(char* fileName)
{
	if (m_pScript)
		delete m_pScript;

	m_pScript = nullptr;
	m_pScript = new LuaScript(fileName);

	return true;
}

void LuaScriptModule::GetElement(float& value, char* elementName)
{
	value = 0.0f;
	if (m_pScript == nullptr)
		return;

	value = m_pScript->get<float>(elementName);
}

void LuaScriptModule::GetElement(int& value, char* elementName)
{
	value = 0.0f;
	if (m_pScript == nullptr)
		return;

	value = m_pScript->get<int>(elementName);
}

void LuaScriptModule::GetElement(char* buffer, char* elementName)
{
	if (m_pScript == nullptr)
		return;

	std::string filename = m_pScript->get<std::string>(elementName);
	strcpy(buffer, filename.c_str());
}

int LuaScriptModule::GetIntArray(int* pArray, char* elementName)
{
	if (m_pScript == nullptr)
		return 0;

	std::vector<int> v = m_pScript->getIntVector("array");

	int vectorSize = v.size();

	int count = 0;
	for (std::vector<int>::Iterator it = v.begin(); it != v.end(); it++) 
	{

		pArray[count] = *it;
		count++;
	}

	return vectorSize;
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