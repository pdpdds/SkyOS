#pragma once
#include "FileSysAdaptor.h"
#include "MintFileSystem.h"

#pragma pack( push, 1 )

// 패키지의 시그너처
#define PACKAGESIGNATURE    "MINT64OSPACKAGE "

// 파일 이름의 최대 길이, 커널의 FILESYSTEM_MAXFILENAMELENGTH와 같음
#define MAXFILENAMELENGTH   24

// 파라미터를 처리하기위해 정보를 저장하는 자료구조
typedef struct kParameterListStruct
{
	// 파라미터 버퍼의 어드레스
	const char* pcBuffer;
	// 파라미터의 길이
	int iLength;
	// 현재 처리할 파라미터가 시작하는 위치
	int iCurrentPosition;
} PARAMETERLIST;

// 패키지 헤더 내부의 각 파일 정보를 구성하는 자료구조
typedef struct PackageItemStruct
{
	// 파일 이름
	char vcFileName[MAXFILENAMELENGTH];

	// 파일의 크기
	DWORD dwFileLength;
} PACKAGEITEM;

// 패키지 헤더 자료구조
typedef struct PackageHeaderStruct
{
	// MINT64 OS의 패키지 파일을 나타내는 시그너처
	char vcSignature[16];

	// 패키지 헤더의 전체 크기
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

