#pragma once
#include "MultiBoot.h"
#include "LoadDLL.h"

typedef LOAD_DLL_INFO* MODULE_HANDLE;

typedef struct DLLInterface
{
	int(*AddNumbers)(int a, int b);
} DLLInterface;

typedef const DLLInterface*(*PGetDLLInterface)();

class SkyModuleManager
{
public:	
	~SkyModuleManager();

	static SkyModuleManager* GetInstance()
	{
		if (m_pModuleManager == 0)
			m_pModuleManager = new SkyModuleManager();

		return m_pModuleManager;
	}

	bool Initialize(multiboot_info* pBootInfo);
	MODULE_HANDLE LoadModuleFromFile(const char* dll_path);
	MODULE_HANDLE LoadModuleFromMemory(const char* moduleName);

	Module* FindModule(multiboot_info* bootinfo, const char* moduleName);
	
	bool UnloadModule(MODULE_HANDLE handle);
	void* GetModuleFunction(MODULE_HANDLE handle, const char* func_name);

private:
	SkyModuleManager();
	static SkyModuleManager* m_pModuleManager;
	multiboot_info* m_pMultibootInfo;
};