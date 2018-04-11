#pragma once
#include "windef.h"
#include "stdint.h"

#define PMM_BLOCKS_PER_BYTE 8	
#define PMM_BLOCK_SIZE	4096	
#define PMM_BLOCK_ALIGN	BLOCK_SIZE
#define PMM_BITS_PER_INDEX	32

namespace PhysicalMemoryManager
{

	void	Initialize(uint32_t memorySize, uint32_t bitmapAddr);

	void SetBit(int bit);
	void UnsetBit(int bit);
	uint32_t GetMemoryMapSize();

	//물리 메모리의 사용 여부에 따라 초기에 프레임들을 Set하거나 Unset한다	
	void	SetAvailableMemory(uint32_t, size_t);
	void	SetDeAvailableMemory(uint32_t base, size_t);

	void*	AllocBlock();
	void	FreeBlock(void*);

	void*	AllocBlocks(size_t);
	void	FreeBlocks(void*, size_t);

	size_t GetMemorySize();

	unsigned int GetFreeFrame();
	unsigned int GetFreeFrames(size_t size);

	uint32_t GetUsedBlockCount();
	uint32_t GetFreeBlockCount();

	uint32_t	GetFreeMemory();

	uint32_t GetTotalBlockCount();
	uint32_t GetBlockSize();

	bool TestMemoryMap(int bit);

	void EnablePaging(bool state);
	bool	IsPaging();

	void LoadPDBR(uint32_t physicalAddr);
	uint32_t GetPDBR();
	
	//Debug
	void Dump();
}	
