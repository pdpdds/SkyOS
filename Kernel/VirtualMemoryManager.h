#pragma once
#include "windef.h"
#include "stdint.h"
#include "PageDirectoryEntry.h"
#include "PageTableEntry.h"


#define KERNEL_VIRTUAL_BASE_ADDRESS 0xC0000000
#define KERNEL_VIRTUAL_HEAP_ADDRESS 0x400000
#define KERNEL_VIRTUAL_PAGEDIRECTORY_ADDRESS 0xCE000000
#define KERNEL_VIRTUAL_STACK_ADDRESS 0xCF000000
#define KERNEL_PHYSICAL_BASE_ADDRESS 0x100000


using namespace PageTableEntry;
using namespace PageDirectoryEntry;

// I86 아키텍쳐에서는 페이지테이블이나 페이지 디렉토리가 각각 1024개의 엔트리를 가진다
// 32비트에서 엔트리의 크기는 4바이트다. 
// 크기(4 * 1024 = 4K)
// 따라서 한 프로세스는 가상주소 4기가를 전부 표현하기 위해 약 4메가바이트의
// 메모리 공간을 필요로 한다(4K(PDE) + 1024 * 4K(PTE))
// 하나의 페이지 테이블은 4MB를 표현할 수 있고 페이지 디렉토리에 1024개의 페이지 테이블이
// 존재하므로 4MB * 1024 = 4GB, 즉 4GB 바이트 전체를 표현할 수 있다.

#define PAGES_PER_TABLE		1024
#define PAGES_PER_DIRECTORY	1024
#define PAGE_TABLE_SIZE		4096

//! page table represents 4mb address space
#define PTABLE_ADDR_SPACE_SIZE 0x400000
//! directory table represents 4gb address space
#define DTABLE_ADDR_SPACE_SIZE 0x100000000
//! page sizes are 4k
#define PAGE_SIZE 4096

// chapter 21
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

typedef struct tag_PageTable 
{
	PTE m_entries[PAGES_PER_TABLE];
}PageTable;

typedef struct tag_PageDirectory 
{
	PDE m_entries[PAGES_PER_DIRECTORY];
}PageDirectory;

typedef struct tag_TaskSwitch
{
	int entryPoint;
	unsigned int procStack;
	LPVOID param;
}TaskSwitch;

namespace VirtualMemoryManager
{
		
	bool Initialize();

	bool AllocPage(PTE* e);
	void FreePage(PTE* e);

	bool SetPageDirectoryInfo(PageDirectory* dir);

	PageDirectory* GetCurPageDirectory();

	//! flushes a cached translation lookaside buffer (TLB) entry
	void FlushTranslationLockBufferEntry(uint32_t addr);
	
//검증해야 됨
	void ClearPageTable(PageTable* p);	
	PTE* GetPTE(PageTable* p, uint32_t addr);
	uint32_t GetPageTableEntryIndex(uint32_t addr);
	
	uint32_t GetPageTableIndex(uint32_t addr);
	void ClearPageDirectory(PageDirectory* dir);	
	PDE* GetPDE(PageDirectory* p, uint32_t addr);
//

	bool CreatePageTable(PageDirectory* dir, uint32_t virt, uint32_t flags);
	void MapPhysicalAddressToVirtualAddresss(PageDirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags);
	void* GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress);
	void UnmapPageTable(PageDirectory* dir, uint32_t virt);
	void UnmapPhysicalAddress(PageDirectory* dir, uint32_t virt);
	PageDirectory* CreateAddressSpace();

//Create Kernel Heap Physical Memory
	bool CreateKernelHeap(PageDirectory* dir);
	bool MapHeap(PageDirectory* dir);	
}

