#include "StorageManager.h"

StorageManager* StorageManager::m_pStorageManager = nullptr;

StorageManager::StorageManager()
{
	memset(m_fileSystems, 0, sizeof(FileSystem*) * STORAGE_DEVICE_MAX);
	m_stroageCount = 0;
	m_pCurrentFileSystem = nullptr;
}

StorageManager::~StorageManager()
{
}

bool StorageManager::RegisterFileSystem(FileSystem* fsys, DWORD deviceID)
{
	if (m_stroageCount < STORAGE_DEVICE_MAX)
	{
		if (fsys) {

			m_fileSystems[deviceID] = fsys;
			m_stroageCount++;

			return true;
		}
	}

	return false;
}

bool StorageManager::UnregisterFileSystem(FileSystem* fsys)
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
	if (deviceID < STORAGE_DEVICE_MAX)
	{
		if (m_fileSystems[deviceID] != nullptr)
		{
			m_pCurrentFileSystem = m_fileSystems[deviceID];
			return true;
		}
	}

	return false;
}

bool StorageManager::SetCurrentFileSystem(FileSystem* fsys)
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
	if (deviceID < STORAGE_DEVICE_MAX)
	{
		if (m_fileSystems[deviceID] != nullptr)
		{
			m_fileSystems[deviceID] = nullptr;
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
	pFile->_deviceID = m_pCurrentFileSystem->_deviceID;
				
	return pFile;
}

int StorageManager::ReadFile(PFILE file, unsigned char* Buffer, unsigned int length)
{
	if (m_pCurrentFileSystem == nullptr)
		return false;

	if (file->_deviceID != m_pCurrentFileSystem->_deviceID)
		return false;

	int len = m_pCurrentFileSystem->Read(file, Buffer, length);

	if (len == 0)
		return false;

	return len;
}

bool StorageManager::CloseFile(PFILE file)
{
	if (m_pCurrentFileSystem == nullptr)
		return false;

	if (file->_deviceID != m_pCurrentFileSystem->_deviceID)
		return false;

	return m_pCurrentFileSystem->Close(file);
}