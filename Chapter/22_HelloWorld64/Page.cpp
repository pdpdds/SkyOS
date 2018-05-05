#include "Page.h"



void InitializePageTable()
{

	PML4ENTRY*	pml4entry  = (PML4ENTRY*) 0x160000;
	PDPTENTRY*	pdptentry  = (PDPTENTRY*) 0x161000;
	PDENTRY*	pdentry    = (PDENTRY*	) 0x162000;

	//P = 1 Rw = 1
	SetPageEntryData(&pml4entry[0], 0x00, 0x161000, PAGE_FLAG_DEFAULT, 0);

	for(int i = 1; i< PAGE_MAX_ENTRY_COUNT; i++)
	{
		SetPageEntryData(&pml4entry[i], 0,0,0,0);
	}
	//P = 1 Rw = 1
	for(int i = 0; i < 64; i++)
	{
		SetPageEntryData(&pdptentry[i], 0, 0x162000 + i * PAGE_TABLE_SIZE,
						 PAGE_FLAG_DEFAULT, 0);
	}

	for(int i=64; i < PAGE_MAX_ENTRY_COUNT; i++)
	{
		SetPageEntryData(&pdptentry[i], 0, 0, 0, 0);
	}

	DWORD LowMapping = 0; 
	
	/*
		'high' for Calculate out of 32bit area. using HighAddressArea 
	*/
	
	for(int i=0; i<PAGE_MAX_ENTRY_COUNT * 64; i++)
	{
		//32비트에서 상위 어드레스를 표현하기 위한 쉬프트 연산 후 계산, 및 
		DWORD high = (i * (PAGE_DEFAULT_SIZE >> 20) ) >> 12;
		SetPageEntryData(&pdentry[i], high, LowMapping, 
						PAGE_FLAG_DEFAULT | PAGE_FLAG_PS, 0);				

		LowMapping += PAGE_DEFAULT_SIZE;
	}
	 

}


void SetPageEntryData(PTENTRY* pEntry, DWORD dwHighBaseAddress, DWORD dwLowBaseAddress,
					  DWORD dwLowFlag, DWORD dwHighFlag)
{
	pEntry->dwLowAddress  = dwLowBaseAddress  | dwLowFlag;
	pEntry->dwHighAddress = (dwHighBaseAddress & 0xFF )| dwHighFlag;
}
