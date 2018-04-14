#include "FileSystem.h"

FileSystem::FileSystem(char* deviceName, DWORD deviceID)
{
	strcpy(m_deviceName, deviceName);
	m_deviceID = deviceID;
}


FileSystem::~FileSystem()
{
}
