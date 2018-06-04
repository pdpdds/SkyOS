#include "FloppyDiskAdaptor.h"
#include "FAT12.h"
#include "FloppyDisk.h"
#include "SkyConsole.h"
#include "SkyStruct.h"

FloppyDiskAdaptor::FloppyDiskAdaptor(char* deviceName, DWORD deviceID)
	: FileSysAdaptor(deviceName, deviceID)
{
}

FloppyDiskAdaptor::~FloppyDiskAdaptor()
{
}

int FloppyDiskAdaptor::GetCount()
{	
	return 0;
}

bool FloppyDiskAdaptor::Initialize()
{
	strcpy(m_fileSys.Name, "FAT12");
	m_fileSys.Directory = fsysFatDirectory;
	m_fileSys.Mount = fsysFatMount;
	m_fileSys.Open = fsysFatOpen;
	m_fileSys.Read = fsysFatRead;
	m_fileSys.Close = fsysFatClose;

	fsysFatMount();
	
	FloppyDisk::SetWorkingDrive(0);
	
	FloppyDisk::Install(38);

	SkyConsole::Print("Floppy Disk Init Complete!!\n");

	return true;
}


int FloppyDiskAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	if (file == nullptr)
		return 0;

	return m_fileSys.Read(file, buffer, size * count);	
}

bool FloppyDiskAdaptor::Close(PFILE file)
{
	if (file == nullptr)
		return false;

	m_fileSys.Close(file);
	delete file;

	return true;
}

PFILE FloppyDiskAdaptor::Open(const char* fileName, const char *mode)
{
	if (fileName == nullptr)
		return nullptr;
	
	FILE file = m_fileSys.Open(fileName);
			
	if(file._flags == FS_INVALID)
		return nullptr;
								
	PFILE fp = new FILE;
	fp->_deviceID = 'A';
	strcpy(fp->_name, fileName);
	fp->_id = file._id;
	fp->_currentCluster = file._currentCluster;
	fp->_position = file._position;
	fp->_flags = file._flags;
	fp->_fileLength = file._fileLength;
	fp->_eof = file._eof;

	return fp;
}

size_t FloppyDiskAdaptor::Write(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	return 0;
}