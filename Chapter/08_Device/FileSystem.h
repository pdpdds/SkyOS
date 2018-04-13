#pragma once
#include "SkyStruct.h"
#include "fileio.h"

//파일 시스템
class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	virtual int Read(PFILE file, unsigned char* buffer, unsigned int length) = 0;
	virtual bool Close(PFILE file) = 0;
	virtual PFILE Open(const char* FileName, const char *mode) = 0;

	char _deviceName[MAXPATH];
	DWORD _deviceID;
};