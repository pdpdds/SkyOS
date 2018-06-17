#include "CDRomAdaptor.h"
#include "SkyConsole.h"
#include "MultiBoot.h"
#include "SkyStruct.h"
#include "PhysicalMemoryManager.h"
#include "Exception.h"

CDRomAdaptor::CDRomAdaptor(char* deviceName, DWORD deviceID)
	: FileSysAdaptor(deviceName, deviceID)
{
	
}

CDRomAdaptor::~CDRomAdaptor()
{
	
}

bool CDRomAdaptor::Initialize()
{

	return false;
}

int CDRomAdaptor::GetCount()
{
	return 1;
}

int CDRomAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	return 0;
}

bool CDRomAdaptor::Close(PFILE file)
{
	return false;
}

PFILE CDRomAdaptor::Open(const char* fileName, const char *mode)
{
	
	return nullptr;
}

size_t CDRomAdaptor::Write(PFILE file, unsigned char* buffer, unsigned int size, int count)
{

		return 0;

	
}
