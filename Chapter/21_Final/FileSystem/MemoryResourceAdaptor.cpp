#include "MemoryResourceAdaptor.h"
#include "SkyConsole.h"
#include "MultiBoot.h"
#include "SkyStruct.h"
#include "PhysicalMemoryManager.h"
#include "SkyModuleManager.h"

MemoryResourceAdaptor::MemoryResourceAdaptor(char* deviceName, DWORD deviceID)
	: FileSysAdaptor(deviceName, deviceID)
{
}


MemoryResourceAdaptor::~MemoryResourceAdaptor()
{
}

bool MemoryResourceAdaptor::Initialize()
{
	SkyConsole::Print("Memory Resource Disk Init Complete\n");
	return true;
}

int MemoryResourceAdaptor::GetCount()
{
	return 1;
}

int MemoryResourceAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	if (file == nullptr)
		return false;

	Module* pModule = (Module*)file->_id;

	int remain = file->_fileLength - file->_position;
	int readCount = size * count;

	if (readCount > remain)
	{
		readCount = remain;
		file->_eof = 1;
	}

	memcpy(buffer, ((char*)pModule->ModuleStart) + file->_position, readCount);

	//SkyConsole::Print("%c", buffer[0]);

	file->_position += readCount;

	return readCount;
}

bool MemoryResourceAdaptor::Close(PFILE file)
{
	if (file == nullptr)
		return false;

	delete file;
	return true;
}

PFILE MemoryResourceAdaptor::Open(const char* fileName, const char *mode)
{
	Module* pModule = SkyModuleManager::GetInstance()->FindModule(fileName);

	if (pModule)
	{
		PFILE file = new FILE;
		file->_deviceID = 'L';
		strcpy(file->_name, fileName);
		file->_id = (DWORD)pModule;
		file->_fileLength = pModule->ModuleEnd - pModule->ModuleStart;
		file->_eof = 0;
		file->_position = 0;
		return file;
	}

	return nullptr;
}

size_t MemoryResourceAdaptor::Write(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	return 0;
}

bool MemoryResourceAdaptor::GetFileList()
{
	SkyModuleManager::GetInstance()->PrintMoudleList();
	return true;
}
