#include "RamDiskAdaptor.h"
#include "SkyConsole.h"
#include "MultiBoot.h"

RamDiskAdaptor::RamDiskAdaptor(char* deviceName, DWORD deviceID)
	: FileSystem(deviceName, deviceID)
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
		InstallPackage();

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

bool RamDiskAdaptor::InstallPackage()
{
	PACKAGEHEADER* pstHeader;
	PACKAGEITEM* pstItem;
	WORD wKernelTotalSectorCount;	
	FILE* fp;
	UINT64 qwDataAddress;

	SkyConsole::Print("Package Install Start...\n");

	// 부트 로더가 로딩된 0x7C05 어드레스에서 보호 모드 커널과 IA-32e 모드 커널을
	// 합한 섹터 수를 읽음
	wKernelTotalSectorCount = *((WORD*)0x7C05);

	// 디스크 이미지는 0x10000 어드레스에 로딩되므로 이를 기준으로
	// 커널 섹터 수만큼 떨어진 곳에 패키지 헤더가 있음
	pstHeader = (PACKAGEHEADER*)((UINT64)KERNEL_LOAD_ADDRESS);

	// 시그너처를 확인
	if (memcmp(pstHeader->vcSignature, PACKAGESIGNATURE,
		sizeof(pstHeader->vcSignature)) != 0)
	{
		SkyConsole::Print("Package Signature Fail\n");
		return false;
	}

	//--------------------------------------------------------------------------
	// 패키지 내의 모든 파일을 찾아서 램 디스크에 복사
	//--------------------------------------------------------------------------
	// 패키지 데이터가 시작하는 어드레스
	qwDataAddress = (UINT64)pstHeader + pstHeader->dwHeaderSize;
	// 패키지 헤더의 첫 번째 파일 데이터
	pstItem = &pstHeader->vstItem;

	// 패키지에 포함된 모든 파일을 찾아서 복사
	for (DWORD i = 0; i < pstHeader->dwHeaderSize / sizeof(PACKAGEITEM); i++)
	{
		SkyConsole::Print("[%d] file: %s, size: %d Byte\n", i + 1, pstItem[i].vcFileName, pstItem[i].dwFileLength);

		// 패키지에 포함된 파일 이름으로 파일을 생성
		fp = fopen(pstItem[i].vcFileName, "w");
		if (fp == NULL)
		{
			SkyConsole::Print("%s File Create Fail\n");
			return false;
		}

		// 패키지 데이터 부분에 포함된 파일 내용을 램 디스크로 복사
		if (fwrite((BYTE*)qwDataAddress, 1, pstItem[i].dwFileLength, fp) != pstItem[i].dwFileLength)
		{
			SkyConsole::Print("Ram Disk Write Fail\n");

			// 파일을 닫고 파일 시스템 캐시를 내보냄
			fclose(fp);

			return false;
		}

		// 파일을 닫음        
		fclose(fp);

		// 다음 파일이 저장된 위치로 이동
		qwDataAddress += pstItem[i].dwFileLength;
	}

	SkyConsole::Print("Package Install Complete\n");
	
	return true;
}