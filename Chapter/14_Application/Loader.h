#pragma once
#include "windef.h"

class Process;

#define PROCESS_GENESIS_ID 101

class Loader
{
public:
	Loader();
	~Loader();

	virtual Process* CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param) = 0;
	virtual Process* CreateProcessFromFile(char* appName, void* param) = 0;

	int GetNextProcessId() { return m_nextProcessId++; }
	static int m_nextProcessId;
};

