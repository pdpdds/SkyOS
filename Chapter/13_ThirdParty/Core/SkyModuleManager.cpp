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

void SkyModuleManager::PrintMoudleList()
{
	uint32_t mb_flags = m_pMultibootInfo->flags;
	if (mb_flags & MULTIBOOT_INFO_MODS)
	{
		uint32_t mods_count = m_pMultibootInfo->mods_count;
		uint32_t mods_addr = (uint32_t)m_pMultibootInfo->Modules;

		for (uint32_t mod = 0; mod < mods_count; mod++)
		{
			Module* module = (Module*)(mods_addr + (mod * sizeof(Module)));

			const char* module_string = (const char*)module->Name;

			SkyConsole::Print(" %s\n", module_string);			
		}
	}
}

Module* SkyModuleManager::FindModule( const char* moduleName)
{
	uint32_t mb_flags = m_pMultibootInfo->flags;
	if (mb_flags & MULTIBOOT_INFO_MODS)
	{
		uint32_t mods_count = m_pMultibootInfo->mods_count;
		uint32_t mods_addr = (uint32_t)m_pMultibootInfo->Modules;

		for (uint32_t mod = 0; mod < mods_count; mod++)
		{
			Module* module = (Module*)(mods_addr + (mod * sizeof(Module)));

			const char* module_string = (const char*)module->Name;

			//SkyConsole::Print("Module Name : %s 0x%x 0x%x\n", module_string, module->ModuleStart, module->ModuleEnd);

			if (strcmp(module_string, moduleName) == 0)
			{
				return module;
			}
		}
	}

	return nullptr;
}