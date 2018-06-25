#pragma once
#include "FileSysAdaptor.h"

struct Module;
class g_ramdisk;

class SkyRamDiskAdaptor : public FileSysAdaptor
{
public:
	SkyRamDiskAdaptor(char* deviceName, DWORD deviceID);
	virtual ~SkyRamDiskAdaptor();

	virtual bool Initialize() override;
	virtual int GetCount() override;
	virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
	virtual bool Close(PFILE file)  override;
	virtual PFILE Open(const char* FileName, const char *mode)  override;
	virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count) override;

	virtual bool GetFileList() override;
protected:
	void LoadRamDisk(Module* ramdiskModule);

private:
	g_ramdisk* m_pRamDisk;
};


