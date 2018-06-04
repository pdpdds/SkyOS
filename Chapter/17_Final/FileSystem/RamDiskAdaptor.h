#pragma once
#include "FileSysAdaptor.h"
#include "MintFileSystem.h"

#pragma pack( push, 1 )

// 패키지의 시그너처
#define PACKAGESIGNATURE    "SKYOS32PACKAGE_  "

// 파일 이름의 최대 길이
#define MAXFILENAMELENGTH   24

// 패키지 내부에 파일 정보를 저장하기 위한 구조체
// 파일 이름
// 파일의 크기
typedef struct tag_PACKAGEITEM
{
	char vcFileName[MAXFILENAMELENGTH];
	DWORD dwFileLength;
} PACKAGEITEM;

// 패키지 헤더 자료구조
// 시그너쳐
// 헤더크기
typedef struct tag_PACKAGEHEADER
{
	char vcSignature[16];
	DWORD dwHeaderSize;
	
} PACKAGEHEADER;

#pragma pack( pop )

class RamDiskAdaptor : public FileSysAdaptor
{
public:
	RamDiskAdaptor(char* deviceName, DWORD deviceID);
	~RamDiskAdaptor();
	
	virtual bool Initialize() override;
	virtual int GetCount() override;
	virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
	virtual bool Close(PFILE file)  override;
	virtual PFILE Open(const char* FileName, const char *mode)  override;
	virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count) override;

	bool InstallPackage();

private:
	void PrintRamDiskInfo();	
	PACKAGEHEADER* FindPackageSignature(UINT32 startAddress, UINT32 endAddress);

private:
	HDDINFORMATION* m_pInformation;
};

