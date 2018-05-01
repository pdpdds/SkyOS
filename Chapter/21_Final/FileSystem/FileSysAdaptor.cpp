#include "FileSysAdaptor.h"
#include "SkyStruct.h"

FileSysAdaptor::FileSysAdaptor(char* deviceName, DWORD deviceID)
{
	strcpy(m_deviceName, deviceName);
	m_deviceID = deviceID;
}


FileSysAdaptor::~FileSysAdaptor()
{
}
