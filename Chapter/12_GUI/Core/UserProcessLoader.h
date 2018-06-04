#pragma once
#include "Loader.h"

class UserProcessLoader : public Loader
{
public:
	UserProcessLoader();
	~UserProcessLoader();

	virtual Process* CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param) override;
	virtual Process* CreateProcessFromFile(char* appName, void* param) override;
};