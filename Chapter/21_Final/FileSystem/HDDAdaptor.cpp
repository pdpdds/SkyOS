#include "SkyOS.h"

HardDiskHandler* g_pHDDHandler = nullptr;

HDDAdaptor::HDDAdaptor(char* deviceName, DWORD deviceID)
	: FileSysAdaptor(deviceName, deviceID)
{
}


HDDAdaptor::~HDDAdaptor()
{
}

int HDDAdaptor::GetCount()
{
	if (g_pHDDHandler)
		return g_pHDDHandler->GetTotalDevices();

	return 0;
}

bool HDDAdaptor::Initialize()
{
	g_pHDDHandler = new HardDiskHandler();
	g_pHDDHandler->Initialize();	

	InitFATFileSystem();	

	if (g_pHDDHandler->GetTotalDevices() == 0)
		return false;

	SkyConsole::Print("HardDisk Count : %d\n", g_pHDDHandler->GetTotalDevices());
	
	PrintHDDInfo();	

	return true;
}

void HDDAdaptor::PrintHDDInfo()
{
	int TotHDD = g_pHDDHandler->GetTotalDevices();
	HDDInfo* pHDDInfo;
	BYTE Key[3] = { 'H','0',0
	};

	for (BYTE i = 0; i < TotHDD; i++)
	{
		pHDDInfo = (HDDInfo *)g_pHDDHandler->GetHDDInfo(Key);
		if (pHDDInfo != NULL)
		{
			char buf[256];
			int index = 0;
			SkyConsole::Print("\n");

			sprintf(buf, "%s Device ( %s ) :: ", pHDDInfo->DeviceNumber ? "Slave " : "Master", Key);
			index += strlen(buf);

			if (pHDDInfo->ModelNumber[0] == 0)
			{
				strcpy(buf + index, " N/A ");	
				index += strlen(" N/A ");
			}
			else
			{
				for (BYTE j = 0; j < 20; j++)
				{
					buf[index] = pHDDInfo->ModelNumber[j];
					index++;
				}
			}

			strcpy(buf + index, " - ");
			index += strlen(" - ");
			
			if (pHDDInfo->SerialNumber[0] == 0)
			{
				strcpy(buf + index, " N/A ");
				index += strlen(" N/A ");
			}
			else
			{
				
				for (BYTE j = 0; j < 20; j++)
				{
					buf[index] = pHDDInfo->SerialNumber[j];
					index++;
				}
				buf[index] = 0;
				SkyConsole::Print(buf);
			}
			SkyConsole::Print("\n");
			SkyConsole::Print("Cylinders %d Heads %d Sectors %d. LBA Sectors %d\n", pHDDInfo->CHSCylinderCount, pHDDInfo->CHSHeadCount, pHDDInfo->CHSSectorCount, pHDDInfo->LBACount);
		}
		Key[1]++;
	}
}

int HDDAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count)
{	
	int readCnt = FATReadFile(file->_id, size * count, buffer);
	//SkyConsole::Print("HDDAdaptor::Read : %d\n", readCnt);
	if (readCnt < (int)(size * count))
		file->_eof = 1;

	file->_position += readCnt;

	return readCnt;
}

bool HDDAdaptor::Close(PFILE file)
{
	return FATFileClose(file->_id);	
}

PFILE HDDAdaptor::Open(const char* fileName, const char *mode)
{		
	std::string name = "C:";
	name += fileName;
	
	int handle = FATFileOpen('C', (char*)name.c_str(), 0);
	
	if (handle == 0)
	{
		SkyConsole::Print("File Open Fail : %s\n", fileName);
		return nullptr;
	}

	PFILE file = new FILE;
	file->_flags = FS_FILE;
	file->_deviceID = 'C';
	strcpy(file->_name, fileName);
	file->_id = handle;
	file->_eof = 0;
	file->_position = 0;
	return file;
}

size_t HDDAdaptor::Write(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	return 0;
}