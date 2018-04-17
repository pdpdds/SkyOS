#pragma once
#include "FileSysAdaptor.h"


typedef struct tag_FLOPPY_FILE_SYSTEM {

	char Name[8];
	FILE(*Directory)  (const char* DirectoryName);
	void(*Mount)      ();
	int(*Read)       (PFILE file, unsigned char* Buffer, unsigned int Length);
	bool(*Close)      (PFILE);
	FILE(*Open)       (const char* FileName);

}FLOPPY_FILESYSTEM, *PFLOPPY_FILESYSTEM;

class FloppyDiskAdaptor : public FileSysAdaptor
{
public:
	FloppyDiskAdaptor(char* deviceName, DWORD deviceID);
	virtual ~FloppyDiskAdaptor();

	
	bool Initialize() override;
	virtual int GetCount() override;
	virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
	virtual bool Close(PFILE file)  override;
	virtual PFILE Open(const char* FileName, const char *mode)  override;
	virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count) override;

private:
	FLOPPY_FILESYSTEM m_fileSys;	
};

