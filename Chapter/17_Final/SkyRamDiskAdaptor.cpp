#include "SkyRamDiskAdaptor.h"
#include "SkyConsole.h"
#include "MultiBoot.h"
#include "SkyStruct.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"
#include "SkyModuleManager.h"
#include "SkyRamDisk.h"
#include "Hal.h"

SkyRamDiskAdaptor::SkyRamDiskAdaptor(char* deviceName, DWORD deviceID)
	: FileSysAdaptor(deviceName, deviceID)
{
	m_pRamDisk = nullptr;
}


SkyRamDiskAdaptor::~SkyRamDiskAdaptor()
{
}

bool SkyRamDiskAdaptor::Initialize()
{
	Module* rd_module = SkyModuleManager::GetInstance()->FindModule("skyramdisk.pak");	
	if (rd_module == 0) {
		//G_PRETTY_BOOT_FAIL("Ramdisk not found (did you supply enough memory?");
		SkyConsole::Print("%s ramdisk not found (did you supply enough memory?)", "kern");
		return false;
	}
	
	SkyConsole::Print("Loading Ramdisk\n");
	LoadRamDisk(rd_module);

	return true;
}

void SkyRamDiskAdaptor::LoadRamDisk(Module* ramdiskModule)
{
	int ramdiskPages = PAGE_ALIGN_UP(ramdiskModule->ModuleEnd - ramdiskModule->ModuleStart) / PAGE_SIZE;

	void* pRamDiskSpace = PhysicalMemoryManager::AllocBlocks(ramdiskPages);

	if (pRamDiskSpace == nullptr)
		return;

	int endAddress = (uint32_t)KERNEL_VIRTUAL_RAMDISK_ADDRESS + ramdiskPages * PMM_BLOCK_SIZE;
	
	for (int i = 0; i < ramdiskPages; i++)
	{
		uint32_t virt = (uint32_t)KERNEL_VIRTUAL_RAMDISK_ADDRESS + i * PAGE_SIZE;
		uint32_t phys = (uint32_t)pRamDiskSpace + i * PAGE_SIZE;

		VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(VirtualMemoryManager::GetCurPageDirectory(), virt, phys, I86_PTE_PRESENT | I86_PTE_WRITABLE);
	}
	memcpy((void*)KERNEL_VIRTUAL_RAMDISK_ADDRESS, (void*)ramdiskModule->ModuleStart, ramdiskModule->ModuleEnd - ramdiskModule->ModuleStart);

	ramdiskModule->ModuleEnd = KERNEL_VIRTUAL_RAMDISK_ADDRESS + (ramdiskModule->ModuleEnd - ramdiskModule->ModuleStart);
	ramdiskModule->ModuleStart = KERNEL_VIRTUAL_RAMDISK_ADDRESS;

	

	m_pRamDisk = new g_ramdisk();
	m_pRamDisk->load(ramdiskModule);
	SkyConsole::Print("%s ramdisk loaded\n", "kern");	
}

int SkyRamDiskAdaptor::GetCount()
{
	return 1;
}

int SkyRamDiskAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	if (file == nullptr)
		return false;

	g_ramdisk_entry* pEntry = (g_ramdisk_entry*)file->_id;

	if (pEntry == nullptr)
		return false;

	int remain = file->_fileLength - file->_position;

	if (remain == 0)
	{
		file->_eof = 1;
		return 0;
	}

	int readCount = size * count;

	if (readCount > remain)
	{
		readCount = remain;
		file->_eof = 1;
	}

	memcpy(buffer, ((char*)pEntry->data) + file->_position, readCount);

	//SkyConsole::Print("%c", buffer[0]);

	file->_position += readCount;

	return readCount;
}

bool SkyRamDiskAdaptor::Close(PFILE file)
{
	if (file == nullptr)
		return false;

	delete file;
	return true;
}

PFILE SkyRamDiskAdaptor::Open(const char* fileName, const char *mode)
{
	if (m_pRamDisk == nullptr)
		nullptr;

	g_ramdisk_entry* pEntry = m_pRamDisk->findAbsolute(fileName);

	if (pEntry == nullptr)
		nullptr;

	if (pEntry)
	{
		PFILE file = new FILE;
		file->_deviceID = 'P';
		strcpy(file->_name, fileName);
		file->_id = (DWORD)pEntry;
		file->_fileLength = pEntry->datalength;
		file->_eof = 0;
		file->_currentCluster = 0;
		file->_position = 0;
		file->_flags = FS_FILE;
		return file;
	}

	return nullptr;
}

size_t SkyRamDiskAdaptor::Write(PFILE file, unsigned char* buffer, unsigned int size, int count)
{
	return 0;
}

bool SkyRamDiskAdaptor::GetFileList()
{
	if (m_pRamDisk == nullptr)
		return false;

	g_ramdisk_entry* pEntry = m_pRamDisk->getFirst();
	if (pEntry == nullptr)
		return false;

	while (pEntry)
	{

		SkyConsole::Print("%s\n", pEntry->name);
		pEntry = pEntry->next;
	}

	return true;
}

