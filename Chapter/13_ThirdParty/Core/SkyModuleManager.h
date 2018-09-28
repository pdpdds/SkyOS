#pragma once
#include "MultiBoot.h"
#include "kheap.h"
#include "list.h"

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
	
	
	Module* FindModule(const char* moduleName);
	void PrintMoudleList();

	multiboot_info* GetMultiBootInfo() { return m_pMultibootInfo; }

private:
	SkyModuleManager();
	static SkyModuleManager* m_pModuleManager;
	multiboot_info* m_pMultibootInfo;	
};