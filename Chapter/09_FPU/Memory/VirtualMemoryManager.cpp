#include "VirtualMemoryManager.h"
#include "PhysicalMemoryManager.h"
#include "string.h"
#include "memory.h"
#include "SkyConsole.h"
#include "MultiBoot.h"	
#include "SkyAPI.h"

namespace VirtualMemoryManager
{
	//! current directory table

	PageDirectory*		_kernel_directory = 0;
	PageDirectory*		_cur_directory = 0;	

	//가상 주소와 매핑된 실제 물리 주소를 얻어낸다.
	void* VirtualMemoryManager::GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress)
	{
		PDE* pagedir = directory->m_entries;
		if (pagedir[virtualAddress >> 22] == 0)
			return NULL;

		return (void*)((uint32_t*)(pagedir[virtualAddress >> 22] & ~0xfff))[virtualAddress << 10 >> 10 >> 12];
	}

	//페이지 디렉토리 엔트리 인덱스가 0이 아니면 이미 페이지 테이블이 존재한다는 의미
	bool CreatePageTable(PageDirectory* dir, uint32_t virt, uint32_t flags)
	{
		PDE* pageDirectory = dir->m_entries;
		if (pageDirectory[virt >> 22] == 0)
		{
			void* pPageTable = PhysicalMemoryManager::AllocBlock();
			if (pPageTable == nullptr)
				return false;					

			memset(pPageTable, 0, sizeof(PageTable));
			pageDirectory[virt >> 22] = ((uint32_t)pPageTable) | flags;					
		}
		return true;
	}

	//PDE나 PTE의 플래그는 같은 값을 공유
	//가상주소를 물리 주소에 매핑
	void MapPhysicalAddressToVirtualAddresss(PageDirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags)
	{
		kEnterCriticalSection();
		PhysicalMemoryManager::EnablePaging(false);
		PDE* pageDir = dir->m_entries;				

		if (pageDir[virt >> 22] == 0)
		{
			CreatePageTable(dir, virt, flags);
		}

		uint32_t mask = (uint32_t)(~0xfff);
		uint32_t* pageTable = (uint32_t*)(pageDir[virt >> 22] & mask);

		pageTable[virt << 10 >> 10 >> 12] = phys | flags;

		PhysicalMemoryManager::EnablePaging(true);
		kLeaveCriticalSection();
	}

	void UnmapPageTable(PageDirectory* dir, uint32_t virt)
	{
		PDE* pageDir = dir->m_entries;
		if (pageDir[virt >> 22] != 0) {

			/* get mapped frame */
			void* frame = (void*)(pageDir[virt >> 22] & 0x7FFFF000);

			/* unmap frame */
			PhysicalMemoryManager::FreeBlock(frame);
			pageDir[virt >> 22] = 0;
		}
	}


	void UnmapPhysicalAddress(PageDirectory* dir, uint32_t virt)
	{
		PDE* pagedir = dir->m_entries;
		if (pagedir[virt >> 22] != 0)
			UnmapPageTable(dir, virt);
	}

	PageDirectory* CreatePageDirectory()
	{
		PageDirectory* dir = NULL;

		/* allocate page directory */
		dir = (PageDirectory*)PhysicalMemoryManager::AllocBlock();
		if (!dir)
			return NULL;

		//memset(dir, 0, sizeof(PageDirectory));

		return dir;
	}

	void ClearPageDirectory(PageDirectory* dir)
	{
		if (dir == NULL)
			return;

		memset(dir, 0, sizeof(PageDirectory));
	}

	PDE* GetPDE(PageDirectory* dir, uint32_t addr)
	{
		if (dir == NULL)
			return NULL;

		return &dir->m_entries[GetPageTableIndex(addr)];
	}

	uint32_t GetPageTableIndex(uint32_t addr)
	{
		return (addr >= DTABLE_ADDR_SPACE_SIZE) ? 0 : addr / PAGE_SIZE;
	}

	//살펴볼 것
	uint32_t GetPageTableEntryIndex(uint32_t addr)
	{

		return (addr >= PTABLE_ADDR_SPACE_SIZE) ? 0 : addr / PAGE_SIZE;
	}

	PTE* GetPTE(PageTable* p, uint32_t addr)
	{
		if (p == NULL)
			return NULL;

		return &p->m_entries[GetPageTableEntryIndex(addr)];
	}

	void ClearPageTable(PageTable* p)
	{
		if (p != NULL)
			memset(p, 0, sizeof(PageTable));
	}

	bool AllocPage(PTE* e)
	{
		void* p = PhysicalMemoryManager::AllocBlock();

		if (p == NULL)
			return false;

		PageTableEntry::SetFrame(e, (uint32_t)p);
		PageTableEntry::AddAttribute(e, I86_PTE_PRESENT);

		return true;
	}

	void FreePage(PTE* e)
	{

		void* p = (void*)PageTableEntry::GetFrame(*e);
		if (p)
			PhysicalMemoryManager::FreeBlock(p);

		PageTableEntry::DelAttribute(e, I86_PTE_PRESENT);
	}


	PageDirectory* CreateCommonPageDirectory()
	{
		
		//페이지 디렉토리 생성. 가상주소 공간 
		//4GB를 표현하기 위해서 페이지 디렉토리는 하나면 충분하다.
		//페이지 디렉토리는 1024개의 페이지테이블을 가진다
		//1024 * 1024(페이지 테이블 엔트리의 개수) * 4K(프레임의 크기) = 4G
		PageDirectory* dir = (PageDirectory*)PhysicalMemoryManager::AllocBlock();

		if (dir == NULL)
			return nullptr;

		memset(dir, 0, sizeof(PageDirectory));

		uint32_t frame = 0x00000000;
		uint32_t virt = 0x00000000;

		//페이지 테이블을 생성
		for (int i = 0; i < 2; i++)
		{			
			PageTable* identityPageTable = (PageTable*)PhysicalMemoryManager::AllocBlock();
			if (identityPageTable == NULL)
			{
				return nullptr;
			}

			memset(identityPageTable, 0, sizeof(PageTable));
			
			//0-128MB 의 물리 주소를 가상 주소와 동일하게 매핑시킨다

		
			for (int j = 0; j < PAGES_PER_TABLE; j++, frame += PAGE_SIZE, virt += PAGE_SIZE)
			{
				PTE page = 0;
				PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);

				PageTableEntry::SetFrame(&page, frame);
				
				identityPageTable->m_entries[PAGE_TABLE_INDEX(virt)] = page;
			}

			//페이지 디렉토리에 페이지 디렉토리 엔트리(PDE)를 한 개 세트한다
			//0번째 인덱스에 PDE를 세트한다(가상주소가 0X00000000일시 참조됨)
			//앞에서 생성한 아이덴티티 페이지 테이블을 세트한다
			//가상주소 = 물리주소
			PDE* identityEntry = &dir->m_entries[PAGE_DIRECTORY_INDEX((virt - 0x00400000))];
			PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_PRESENT | I86_PDE_WRITABLE);			
			PageDirectoryEntry::SetFrame(identityEntry, (uint32_t)identityPageTable);
		}

		return dir;
	}	

	bool Initialize()
	{
		SkyConsole::Print("Virtual Memory Manager Init..\n");

		PageDirectory* dir = CreateCommonPageDirectory();

		if (nullptr == dir)
			return false;

		//페이지 디렉토리를 PDBR 레지스터에 로드한다
		SetCurPageDirectory(dir);
		SetKernelPageDirectory(dir);

		_asm
		{
			mov	eax, [dir]
			mov	cr3, eax		// PDBR is cr3 register in i86
		}

		//페이징 기능을 다시 활성화시킨다
		PhysicalMemoryManager::EnablePaging(true);
		
		return true;
	}

	bool SetKernelPageDirectory(PageDirectory* dir)
	{
		if (dir == NULL)
			return false;

		_kernel_directory = dir;

		return true;
	}

	PageDirectory* GetKernelPageDirectory()
	{
		return _kernel_directory;
	}



	bool SetCurPageDirectory(PageDirectory* dir)
	{
		if (dir == NULL)
			return false;

		_cur_directory = dir;		

		return true;
	}

	PageDirectory* GetCurPageDirectory()
	{
		return _cur_directory;
	}

	void FlushTranslationLockBufferEntry(uint32_t addr)
	{
#ifdef _MSC_VER
		_asm {
			cli
			invlpg	addr
			sti
		}
#endif
	}	

	void Dump()
	{
		
	}
}