#pragma once
#include "FileSysAdaptor.h"

class MemoryResourceAdaptor : public FileSysAdaptor
{
public:
	MemoryResourceAdaptor(char* deviceName, DWORD deviceID);
	~MemoryResourceAdaptor();

	virtual bool Initialize() override;
	virtual int GetCount() override;
	virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
	virtual bool Close(PFILE file)  override;
	virtual PFILE Open(const char* FileName, const char *mode)  override;
	virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count) override;

	virtual bool GetFileList() override;
};

