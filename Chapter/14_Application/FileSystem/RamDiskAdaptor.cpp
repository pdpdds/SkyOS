#include "RamDiskAdaptor.h"
#include "SkyConsole.h"
#include "MultiBoot.h"

RamDiskAdaptor::RamDiskAdaptor(char* deviceName, DWORD deviceID)
	: FileSysAdaptor(deviceName, deviceID)
{
	m_pInformation = new HDDINFORMATION;
}

RamDiskAdaptor::~RamDiskAdaptor()
{
	delete m_pInformation;
}

bool RamDiskAdaptor::Initialize()
{
	bool result = kInitializeRDDFileSystem();	

	if (result == true)
	{
		kGetHDDInformation(m_pInformation);

		PrintRamDiskInfo();		

		return true;
	}

	return false;
}

int RamDiskAdaptor::GetCount()
{	
	return 1;
}


void RamDiskAdaptor::PrintRamDiskInfo()
{
	SkyConsole::Print("RamDisk Info\n");
	SkyConsole::Print("Total Sectors : %d\n", m_pInformation->dwTotalSectors);
	SkyConsole::Print("Serial Number : %s\n", m_pInformation->vwSerialNumber);
	SkyConsole::Print("Model Number : %s\n", m_pInformation->vwModelNumber);	 
}

int RamDiskAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	if (file == nullptr)
		return false;

	return kReadFile(buffer, size, count, (MFILE*)file->_id);
}

bool RamDiskAdaptor::Close(PFILE file)
{
	if (file == nullptr)
		return false;

	return (-1 != kCloseFile((MFILE*)file->_id));	
}

PFILE RamDiskAdaptor::Open(const char* fileName, const char *mode)
{
	MFILE* pMintFile = kOpenFile(fileName, mode);

	if (pMintFile)
	{
		PFILE file = new FILE;
		file->_deviceID = 'C';
		strcpy(file->_name, fileName);
		file->_id = (DWORD)pMintFile;
		return file;
	}

	return nullptr;
}

size_t RamDiskAdaptor::Write(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	if (file == nullptr)
		return 0;
	
	return kWriteFile(buffer, size, count, (MFILE*)file->_id);
}

PACKAGEHEADER* RamDiskAdaptor::FindPackageSignature(UINT32 startAddress, UINT32 endAddress)
{
	// 디스크 이미지는 0x10000 어드레스에 로딩되므로 이를 기준으로
	// 커널 섹터 수만큼 떨어진 곳에 패키지 헤더가 있음
	PACKAGEHEADER* pstHeader = nullptr;
	
	for (UINT32 addr = startAddress; addr < endAddress; addr += 512)
	{
		pstHeader = (PACKAGEHEADER*)(addr);
		// 시그너처를 확인
		if (memcmp(pstHeader->vcSignature, PACKAGESIGNATURE, sizeof(pstHeader->vcSignature)) == 0)
		{			
			return pstHeader;
		}				
	}		
	return nullptr;
}

bool RamDiskAdaptor::InstallPackage()
{	
	FILE* fp;
	PACKAGEITEM* pstItem = nullptr;
	UINT32 dwDataAddress = 0;

	SkyConsole::Print("Package Install Start...\n");

	PACKAGEHEADER* pstHeader = FindPackageSignature(KERNEL_LOAD_ADDRESS, KERNEL_END_ADDRESS);

	if(pstHeader == nullptr)
	{
		SkyConsole::Print("Package Signature Fail\n");
		return false;
	}	
	//--------------------------------------------------------------------------
	// 패키지 내의 모든 파일을 찾아서 램 디스크에 복사
	//--------------------------------------------------------------------------
	// 패키지 데이터가 시작하는 어드레스
	dwDataAddress = (UINT32)(((char*)pstHeader) + sizeof(PackageHeaderStruct));		
	// 패키지 헤더의 첫 번째 파일 데이터
	pstItem = (PACKAGEITEM*)dwDataAddress;

	// 패키지에 포함된 모든 파일을 찾아서 복사
	for (DWORD i = 0; i < pstHeader->dwHeaderSize / sizeof(PACKAGEITEM); i++)
	{
		SkyConsole::Print("[%d] file: %s, size: %d Byte\n", i + 1, pstItem[i].vcFileName, (int)(pstItem[i].dwFileLength));

		// 패키지에 포함된 파일 이름으로 파일을 생성
		fp = fopen(pstItem[i].vcFileName, "w");
		if (fp == NULL)
		{
			SkyConsole::Print("%s File Create Fail\n");
			return false;
		}
		
		// 패키지 데이터 부분에 포함된 파일 내용을 램 디스크로 복사
		if (fwrite((BYTE*)dwDataAddress, 1, pstItem[i].dwFileLength, fp) != pstItem[i].dwFileLength)
		{
			SkyConsole::Print("Ram Disk Write Fail\n");

			// 파일을 닫고 파일 시스템 캐시를 내보냄
			fclose(fp);

			return false;
		}		

		// 파일을 닫음        
		fclose(fp);

		// 다음 파일이 저장된 위치로 이동
		dwDataAddress += pstItem[i].dwFileLength;
	}

	SkyConsole::Print("Package Install Complete\n");
	
	return true;
}