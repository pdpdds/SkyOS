#pragma once
#include "fileio.h"

//파일 시스템
class FileSysAdaptor
{
public:
	FileSysAdaptor(char* deviceName, DWORD deviceID);
	virtual ~FileSysAdaptor();

	virtual FILE* Open(const char* FileName, const char *mode) = 0;
	virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count) = 0;	
	virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count) = 0;
	virtual bool Close(PFILE file) = 0;

	virtual bool Initialize() = 0;
	virtual int GetCount() = 0;

	virtual bool GetFileList() { return false; }

	char m_deviceName[MAXPATH];
	DWORD m_deviceID;
};