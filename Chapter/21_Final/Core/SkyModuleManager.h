#pragma once
#include "MultiBoot.h"
#include "LoadDLL.h"
#include "kheap.h"
#include "list.h"

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

	LOAD_DLL_INFO* FindLoadedModule(const char* dll_path);
	Module* FindModule(const char* moduleName);
	void PrintMoudleList();
	
	bool UnloadModule(MODULE_HANDLE handle);
	void* GetModuleFunction(MODULE_HANDLE handle, const char* func_name);

	multiboot_info* GetMultiBootInfo() { return m_pMultibootInfo; }

private:
	SkyModuleManager();
	static SkyModuleManager* m_pModuleManager;
	multiboot_info* m_pMultibootInfo;
	list<LOAD_DLL_INFO*> m_moduleList;
};