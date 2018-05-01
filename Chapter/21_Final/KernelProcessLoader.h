#pragma once
#include "Loader.h"

class KernelProcessLoader : public Loader
{
public:
	KernelProcessLoader();
	~KernelProcessLoader();

	virtual Process* CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param) override;
	virtual Process* CreateProcessFromFile(char* appName, void* param) override;
};

