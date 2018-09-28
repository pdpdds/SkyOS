#include "SkyOS.h"
#include "SkyTest.h"

StorageManager* StorageManager::m_pStorageManager = nullptr;

FILE* g_stdOut = nullptr;
FILE* g_stdIn = nullptr;
FILE* g_stdErr = nullptr;

StorageManager::StorageManager()
{
	memset(m_fileSystems, 0, sizeof(FileSysAdaptor*) * STORAGE_DEVICE_MAX);
	m_stroageCount = 0;
	m_pCurrentFileSystem = nullptr;

	g_stdOut = new FILE;
	g_stdIn = new FILE;
	g_stdErr = new FILE;
	strcpy(g_stdOut->_name, "STDOUT");
	strcpy(g_stdIn->_name, "STDIN");
	strcpy(g_stdErr->_name, "STDERR");
}

StorageManager::~StorageManager()
{
}

bool StorageManager::RegisterFileSystem(FileSysAdaptor* fsys, DWORD deviceID)
{
	char deviceLetter = toupper(deviceID);
	deviceLetter -= 'A';

	if (m_stroageCount < STORAGE_DEVICE_MAX)
	{
		if (fsys) {

			m_fileSystems[deviceLetter] = fsys;
			fsys->m_deviceID = deviceLetter;
			m_stroageCount++;

			return true;
		}
	}

	return false;
}

bool StorageManager::UnregisterFileSystem(FileSysAdaptor* fsys)
{
	for (int i = 0; i < STORAGE_DEVICE_MAX; i++)
	{
		if (m_fileSystems[i] == fsys)
		{
			m_fileSystems[i] = nullptr;
			m_stroageCount--;
			return true;
		}
	}

	return false;
}

bool StorageManager::SetCurrentFileSystemByID(DWORD deviceID)
{
	char deviceLetter = toupper(deviceID);
	deviceLetter -= 'A';
	if (deviceLetter < STORAGE_DEVICE_MAX)
	{
		if (m_fileSystems[deviceLetter] != nullptr)
		{
			m_pCurrentFileSystem = m_fileSystems[deviceLetter];
			return true;
		}
	}

	return false;
}

bool StorageManager::SetCurrentFileSystem(FileSysAdaptor* fsys)
{
	for (int i = 0; i < STORAGE_DEVICE_MAX; i++)
	{
		if (m_fileSystems[i] == fsys)
		{
			m_pCurrentFileSystem = m_fileSystems[i];
			return true;
		}
	}
	return false;
}

bool StorageManager::UnregisterFileSystemByID(DWORD deviceID)
{
	char deviceLetter = toupper(deviceID);
	deviceLetter -= 'A';
	if (deviceLetter < STORAGE_DEVICE_MAX)
	{
		if (m_fileSystems[deviceLetter] != nullptr)
		{
			m_fileSystems[deviceLetter] = nullptr;
			return true;
		}
	}

	return false;
}

PFILE StorageManager::OpenFile(const char* fileName, const char *mode)
{		
	if (m_pCurrentFileSystem == nullptr || fileName == nullptr)
		return nullptr;
	
	//파일을 사용할 수 있으면 디바이스 아이디를 세팅하고 리턴한다.		
	PFILE pFile = m_pCurrentFileSystem->Open(fileName, mode);	
	
	if (pFile == nullptr)
		return nullptr;
	
	strcpy(pFile->_name, fileName);
	pFile->_deviceID = m_pCurrentFileSystem->m_deviceID;
	return pFile;
}

int StorageManager::ReadFile(PFILE file, unsigned char* Buffer, unsigned int size, int count)
{
	if (count == 0)
		return 0;

	if (m_pCurrentFileSystem == nullptr)
		return 0;

	if (file->_deviceID != m_pCurrentFileSystem->m_deviceID)
		return 0;

	return m_pCurrentFileSystem->Read(file, Buffer, size, count);	
}

int StorageManager::WriteFile(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	if (strcmp(file->_name, "STDOUT") == 0)
	{
		SkyConsole::Print("%s", buffer);
		return size * count;
	}

	if (m_pCurrentFileSystem == nullptr)
		return false;

	if (file->_deviceID != m_pCurrentFileSystem->m_deviceID)
		return false;

	int len = m_pCurrentFileSystem->Write(file, buffer, size, count);

	if (len == 0)
		return 0;

	return len;
}

bool StorageManager::CloseFile(PFILE file)
{
	if (m_pCurrentFileSystem == nullptr)
		return false;

	if (file->_deviceID != m_pCurrentFileSystem->m_deviceID)
		return false;

	return m_pCurrentFileSystem->Close(file);
}

bool StorageManager::GetFileList()
{
	if (m_pCurrentFileSystem == nullptr)
		return false;

	return m_pCurrentFileSystem->GetFileList();
}

bool StorageManager::Initilaize(multiboot_info* info)
{
	if (m_stroageCount > 0)
		return false;

	return ConstructFileSystem(info);
}

bool StorageManager::ConstructFileSystem(multiboot_info* info)
{
	//IDE 하드 디스크
	FileSysAdaptor* pHDDAdaptor = new HDDAdaptor("HardDisk", 'C');

	pHDDAdaptor->Initialize();

	if (pHDDAdaptor->GetCount() > 0)
	{
		StorageManager::GetInstance()->RegisterFileSystem(pHDDAdaptor, 'C');	
	}
	else
	{
		delete pHDDAdaptor;
	}
	
	//램 디스크
	FileSysAdaptor* pRamDiskAdaptor = new RamDiskAdaptor("RamDisk", 'K');
	if (pRamDiskAdaptor->Initialize() == true)
	{
		StorageManager::GetInstance()->RegisterFileSystem(pRamDiskAdaptor, 'K');
		StorageManager::GetInstance()->SetCurrentFileSystemByID('K');
		((RamDiskAdaptor*)pRamDiskAdaptor)->InstallPackage();
	}
	else
	{
		delete pRamDiskAdaptor;
	}

	//메모리 리소스 디스크
	FileSysAdaptor* pMemoryResouceAdaptor = new MemoryResourceAdaptor("MemoryResouceDisk", 'L');
	if (pMemoryResouceAdaptor->Initialize() == true)
	{
		StorageManager::GetInstance()->RegisterFileSystem(pMemoryResouceAdaptor, 'L');
	}
	else
	{
		delete pMemoryResouceAdaptor;
	}

	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');
	SkyConsole::Print("L drive Selected\n");


	return true;
}
