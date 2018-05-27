#include "RamDiskAdaptor.h"
#include "SkyConsole.h"
#include "MultiBoot.h"
#include "SkyStruct.h"
#include "PhysicalMemoryManager.h"

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

	int readCount = kReadFile(buffer, size, count, (MFILE*)file->_id);

	if (readCount < size * count)
		file->_eof = 1;

	file->_position += readCount;

	return readCount;
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
		file->_deviceID = 'K';
		strcpy(file->_name, fileName);
		file->_id = (DWORD)pMintFile;
		file->_fileLength = pMintFile->stFileHandle.dwFileSize;
		file->_eof = 0;
		file->_position = 0;
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

//패키지 데이터를 파싱해서 모든 파일 데이터를 램디스크로 복사
bool RamDiskAdaptor::InstallPackage()
{	
	FILE* fp;
	PACKAGEITEM* pstItem = nullptr;
	UINT32 dwDataAddress = 0;

	//패키지 시그너쳐를 찾는다. 시그너쳐 : "SKYOS32PACKAGE "
	PACKAGEHEADER* pstHeader = FindPackageSignature(KERNEL_LOAD_ADDRESS, PhysicalMemoryManager::GetKernelEnd());

	if(pstHeader == nullptr)
	{		
		return false;
	}	

	// 패키지 데이터 포인터
	dwDataAddress = (UINT32)(((char*)pstHeader) + pstHeader->dwHeaderSize);
	// 패키지아이템 구조체 포인터
	pstItem = (PACKAGEITEM*)(((char*)pstHeader) + sizeof(PACKAGEHEADER));
	
	DWORD dwItemCount = (pstHeader->dwHeaderSize - sizeof(PACKAGEHEADER)) / sizeof(PACKAGEITEM);
	// 패키지에 포함된 모든 파일을 찾아서 복사
	for (DWORD i = 0; i <  dwItemCount; i++)
	{
		SkyConsole::Print("[%d] file: %s, size: %d Byte\n", i + 1, pstItem[i].vcFileName, (int)(pstItem[i].dwFileLength));

		// 패키지에 포함된 파일 이름으로 파일을 생성
		fp = fopen(pstItem[i].vcFileName, "w");
		if (fp == NULL)
		{
			SkyConsole::Print("%s File Create Fail\n");
			return false;
		}
		
		// 파일 내용을 램 디스크로 복사
		if (fwrite((BYTE*)dwDataAddress, 1, pstItem[i].dwFileLength, fp) != pstItem[i].dwFileLength)
		{
			SkyConsole::Print("Ram Disk Write Fail\n");

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