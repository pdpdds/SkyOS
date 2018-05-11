#include "SkyModuleManager.h"
#include "memory.h"
#include "string.h"
#include "SkyConsole.h"
#include "ProcessUtil.h"

SkyModuleManager* SkyModuleManager::m_pModuleManager = nullptr;

SkyModuleManager::SkyModuleManager()
{
}

SkyModuleManager::~SkyModuleManager()
{
}

bool SkyModuleManager::Initialize(multiboot_info* pBootInfo)
{
	m_pMultibootInfo = pBootInfo;
	return true;
}

Module* SkyModuleManager::FindModule(multiboot_info* bootinfo, const char* moduleName)
{
	uint32_t mb_flags = bootinfo->flags;
	if (mb_flags & MULTIBOOT_INFO_MODS)
	{
		uint32_t mods_count = bootinfo->mods_count;   
		uint32_t mods_addr = (uint32_t)bootinfo->Modules;     

		for (uint32_t mod = 0; mod < mods_count; mod++)
		{
			Module* module = (Module*)(mods_addr + (mod * sizeof(Module)));    

			const char* module_string = (const char*)module->Name;

			SkyConsole::Print("Module Name : %s 0x%x 0x%x\n", module_string, module->ModuleStart, module->ModuleEnd);

			if (strcmp(module_string, moduleName) == 0)
			{
				return module;
			}
		}
	}

	return nullptr;
}

MODULE_HANDLE SkyModuleManager::LoadModuleFromFile(const char* dll_path)
{
	LOAD_DLL_INFO* p = new LOAD_DLL_INFO;
	DWORD res = LoadDLLFromFileName(dll_path, 0, p);
	if (res != ELoadDLLResult_OK)
	{
		delete p;
		return NULL;
	}
	return p;
}

MODULE_HANDLE SkyModuleManager::LoadModuleFromMemory(const char* moduleName)
{
	Module* pModule = FindModule(m_pMultibootInfo, moduleName);

	if (pModule == nullptr)
		return nullptr;
		
	SkyConsole::Print("Found %s\n", pModule->Name);

	if (false == ValidatePEImage((void*)pModule->ModuleStart))
	{
		SkyConsole::Print("invalid %s\n", pModule->Name);
		return nullptr;
	}

	LOAD_DLL_INFO* p = new LOAD_DLL_INFO;
	DWORD res = LoadDLLFromMemory((void*)pModule->ModuleStart, ((size_t)(pModule->ModuleEnd) - (size_t)pModule->ModuleStart), 0, p);
	if (res != ELoadDLLResult_OK)
	{
		delete p;
		SkyConsole::Print("LoadModuleFromMemory Fail. ModuleName : %s Error : %d\n", pModule->Name, res);
	}

	SkyConsole::Print("%s Module Loaded\n", moduleName);

	return p;
}

bool SkyModuleManager::UnloadModule(MODULE_HANDLE handle)
{
	bool res = FALSE != UnloadDLL(handle);
	delete handle;
	return res;
}

void* SkyModuleManager::GetModuleFunction(MODULE_HANDLE handle, const char* func_name)
{
	return (void*)myGetProcAddress_LoadDLLInfo(handle, func_name);
}


