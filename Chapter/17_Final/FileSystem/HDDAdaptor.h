#pragma once
#include "FileSysAdaptor.h"
#include "HardDisk.h"
#include "fat32.h"

class HDDAdaptor : public FileSysAdaptor
{
public:
	HDDAdaptor(char* deviceName, DWORD deviceID);
	virtual ~HDDAdaptor();
	
	void PrintHDDInfo();

	bool Initialize() override;
	virtual int GetCount() override;
	virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
	virtual bool Close(PFILE file)  override;
	virtual PFILE Open(const char* FileName, const char *mode)  override;
	virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
	virtual bool GetFileList() override;

private:
	
};