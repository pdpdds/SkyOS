#include "PhysicalMemoryManager.h"
#include "string.h"
#include "memory.h"
#include "SkyConsole.h"

extern uint32_t g_freeMemoryStartAddress;

namespace PhysicalMemoryManager
{
	uint32_t	m_memorySize = 0;
	uint32_t	m_usedBlocks = 0;

	//이용할 수 있는 최대 블럭 갯수
	uint32_t	m_maxBlocks = 0;

	//비트맵 배열, 각 비트는 메모리 블럭을 표현, 비트맵처리
	uint32_t*	m_pMemoryMap = 0;
	uint32_t	m_memoryMapSize = 0;	

	// memorySize : 전체 메모리의 크기(바이트 사이즈)
	//bitmapAddr : 커널다음에 배치되는 비트맵 배열
	//이 배열을 참조해서 해당 물리 메모리가 할당되었는지 사용중인지를 판단한다.
	void Initialize(uint32_t memorySize, uint32_t bitmapAddr)
	{
		SkyConsole::Print("Physical Memory Manager Init..\n");

		m_usedBlocks = 0;
		m_memorySize = memorySize;
		m_maxBlocks = m_memorySize / PMM_BLOCK_SIZE;
		m_pMemoryMap = (uint32_t*)bitmapAddr;

		//블럭들의 최대 수는 8의 배수로 맞추고 나머지는 버린다
		//m_maxBlocks = m_maxBlocks - (m_maxBlocks % PMM_BLOCKS_PER_BYTE);

		//메모리맵의 바이트크기
		m_memoryMapSize = m_maxBlocks / PMM_BLOCKS_PER_BYTE;
		m_usedBlocks = GetTotalBlockCount();

		int tempMemoryMapSize = (GetMemoryMapSize() / 4096) * 4096;

		if (GetMemoryMapSize() % 4096 > 0)
			tempMemoryMapSize += 4096;

		m_memoryMapSize = tempMemoryMapSize;

		//모든 메모리 블럭들이 사용중에 있다고 설정한다.	
		unsigned char flag = 0xff;
		memset((char*)m_pMemoryMap, flag, m_memoryMapSize);

		SetAvailableMemory(g_freeMemoryStartAddress, memorySize);
	}

	uint32_t GetMemoryMapSize() { return m_memoryMapSize; }

	//8번째 메모리 블럭이 사용중임을 표시하기 위해 1로 세팅하려면 배열 첫번째 요소(4바이트) 바이트의 8번째 비트에 접근해야 한다
	void SetBit(int bit)
	{
		m_pMemoryMap[bit / 32] |= (1 << (bit % 32));
	}

	//8번째 메모리 블럭을 사용할 수 있음을 표시하기 위해 0으로 세팅하려면 배열 첫번째 요소(4바이트) 바이트의 8번째 비트에 접근해야 한다
	//00000001000000000000000000000000
	//11111110111111111111111111111111
	//AND 연산자에 의해 8번째 비트를 제외한 비트는 모두 원래 값을 유지한다
	void UnsetBit(int bit)
	{
		m_pMemoryMap[bit / 32] &= ~(1 << (bit % 32));
	}

	void LoadPDBR(uint32_t physicalAddr)
	{
#ifdef _MSC_VER
		_asm
		{
			mov	eax, [physicalAddr]
			mov	cr3, eax		// PDBR is cr3 register in i86
		}
#endif
	}

	uint32_t GetPDBR()
	{

#ifdef _MSC_VER
		_asm
		{
			mov	eax, cr3
			ret
		}
#endif
	}

	bool IsPaging()
	{
		uint32_t res = 0;

#ifdef _MSC_VER
		_asm {
			mov	eax, cr0
			mov[res], eax
		}
#endif

		return (res & 0x80000000) ? false : true;
	}

	void EnablePaging(bool state)
	{
#ifdef _MSC_VER
		_asm
		{
			mov	eax, cr0
			cmp[state], 1
			je	enable
			jmp disable
			enable :
			or eax, 0x80000000		//set bit 31
				mov	cr0, eax
				jmp done
				disable :
			and eax, 0x7FFFFFFF		//clear bit 31
				mov	cr0, eax
				done :
		}
#endif
	}

	void* AllocBlock() {

		if (GetFreeBlockCount() <= 0)
			return NULL;

		unsigned int frame = GetFreeFrame();

		if (frame == -1)
			return NULL;

		SetBit(frame);
		//SkyConsole::Print("free frame : 0x%x\n", frame);

		uint32_t addr = frame * PMM_BLOCK_SIZE + g_freeMemoryStartAddress;
		m_usedBlocks++;

		return (void*)addr;
	}

	void FreeBlock(void* p) {

		uint32_t addr = (uint32_t)p;
		int frame = addr / PMM_BLOCK_SIZE;

		UnsetBit(frame);

		m_usedBlocks--;
	}

	
	void* AllocBlocks(size_t size)
	{
		if (GetFreeBlockCount() <= size)
		{
			return NULL;
		}

		
		int frame = GetFreeFrames(size);

		if (frame == -1)
		{
			return NULL;	//연속된 빈 블럭들이 존재하지 않는다.
		}

		for (uint32_t i = 0; i < size; i++)
			SetBit(frame + i);

		uint32_t addr = frame * PMM_BLOCK_SIZE + g_freeMemoryStartAddress;
		m_usedBlocks += size;

		return (void*)addr;
	}

	void FreeBlocks(void* p, size_t size) {

		uint32_t addr = (uint32_t)p - g_freeMemoryStartAddress;
		int frame = addr / PMM_BLOCK_SIZE;

		for (uint32_t i = 0; i < size; i++)
			UnsetBit(frame + i);

		m_usedBlocks -= size;
	}

	size_t GetMemorySize() {

		return m_memorySize;
	}

	uint32_t GetUsedBlockCount() {

		return m_usedBlocks;
	}

	uint32_t GetFreeBlockCount() {

		return m_maxBlocks - m_usedBlocks;
	}

	uint32_t	GetFreeMemory()
	{
		return GetFreeBlockCount() * PMM_BLOCK_SIZE;
	}

	uint32_t GetTotalBlockCount() {

		return m_maxBlocks;
	}

	uint32_t GetBlockSize()
	{
		return PMM_BLOCK_SIZE;
	}

	//해당 비트가 세트되어 있는지 되어 있지 않은지를 체크한다
	//비트가 유효값을 벗어나면 0을 리턴한다.
	bool TestMemoryMap(int bit)
	{
		return (m_pMemoryMap[bit / 32] & (1 << (bit % 32))) > 0;
	}

	//비트가 0인 프레임 인덱스를 얻어낸다(사용할 수 있는 빈 블럭 비트 인덱스)
	unsigned int GetFreeFrame()
	{
		for (uint32_t i = 0; i < GetTotalBlockCount() / 32; i++)
		{
			if (m_pMemoryMap[i] != 0xffffffff)
				for (unsigned int j = 0; j < PMM_BITS_PER_INDEX; j++)
				{
					unsigned int bit = 1 << j;
					if ((m_pMemoryMap[i] & bit) == 0)
						return i * PMM_BITS_PER_INDEX + j;
				}
		}

		return 0xffffffff;
	}

	//연속된 빈 프레임(블럭)들을 얻어낸다
	unsigned int GetFreeFrames(size_t size)
	{
		if (size == 0)
			return 0xffffffff;

		if (size == 1)
			return GetFreeFrame();

		

		for (uint32_t i = 0; i < GetTotalBlockCount() / 32; i++)
		{		
			if (m_pMemoryMap[i] != 0xffffffff)
			{				
				for (unsigned int j = 0; j < 32; j++)
				{
					unsigned int bit = 1 << j;
					if ((m_pMemoryMap[i] & bit) == 0)
					{
						
						unsigned int startingBit = i * PMM_BITS_PER_INDEX + j;

						// 연속된 빈 프레임의 갯수를 증가시킨다.

						uint32_t free = 0;
						for (uint32_t count = 0; count < size; count++)
						{
							//메모리맵을 벗어나는 상황
							if (startingBit + count >= m_maxBlocks)
								return 0xffffffff;

							if (TestMemoryMap(startingBit + count) == false)
								free++;
							else 
								break;

							//연속된 빈 프레임들이 존재한다. 시작 비트 인덱스는 startingBit
							if (free == size)
								return startingBit;
						}
					}
				}
			}
		}

		return 0xffffffff;
	}

	void SetAvailableMemory(uint32_t base, size_t size)
	{
				
		int usedBlock = GetMemoryMapSize() / PMM_BLOCK_SIZE;
		int blocks = GetTotalBlockCount();

		for (int i = usedBlock; i < blocks; i++) {
			UnsetBit(i);
			m_usedBlocks--;
		}		
	}
	void SetDeAvailableMemory(uint32_t base, size_t size)
	{
		int align = base / PMM_BLOCK_SIZE;
		int blocks = size / PMM_BLOCK_SIZE;

		for (; blocks > 0; blocks--) {
			SetBit(align++);
			m_usedBlocks++;
		}
	}

	void Dump()
	{
		SkyConsole::Print("Memory Size : 0x%x\n", m_memorySize);
		SkyConsole::Print("Memory Map Address : 0x%x\n", m_pMemoryMap);
		SkyConsole::Print("Memory Map Size : %d bytes\n", m_memoryMapSize);
		SkyConsole::Print("Max Block Count : %d\n", m_maxBlocks);

		SkyConsole::Print("Used Block Count : %d\n", m_usedBlocks);
	}
}