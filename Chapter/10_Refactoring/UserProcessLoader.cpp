#include "UserProcessLoader.h"
#include "Process.h"


UserProcessLoader::UserProcessLoader()
{
}


UserProcessLoader::~UserProcessLoader()
{
}

Process* UserProcessLoader::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param)
{
	return nullptr;
}
Process* UserProcessLoader::CreateProcessFromFile(char* appName, void* param)
{
	return nullptr;
}
