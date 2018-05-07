#pragma once
#include "windef.h"

#pragma pack(push,1)

#define PAGE_FLAG_P			0x00000001
#define PAGE_FLAG_RW		0x00000002
#define PAGE_FLAG_US		0x00000004
#define PAGE_FLAG_PWT		0x00000008
#define PAGE_FLAG_PCD		0x00000010
#define PAGE_FLAG_A			0x00000020
#define PAGE_FLAG_D			0x00000040
#define PAGE_FLAG_PS		0x00000080
#define PAGE_FLAG_G			0x00000100
#define PAGE_FLAG_PAT		0x00001000	
#define PAGE_FLAG_EXB		0x80000000	
#define PAGE_TABLE_SIZE		0x1000 //4KB
#define PAGE_DEFAULT_SIZE	0X200000
#define PAGE_FLAG_DEFAULT	(PAGE_FLAG_P | PAGE_FLAG_RW)
#define PAGE_MAX_ENTRY_COUNT		512

typedef struct __Struct_PageEntry
{		
	/*0----------------31 bit */
	DWORD dwLowAddress;
	/*32---------------64 bit */
	DWORD dwHighAddress;

} PML4ENTRY, PDPTENTRY, PDENTRY, PTENTRY;	

#pragma pack(pop)

void InitializePageTable();
void SetPageEntryData(PTENTRY* pEntry, DWORD dwHighBaseAddress, 
					  DWORD dwLowBaseAddress, DWORD dwLowFlag, DWORD dwHighFlag);
