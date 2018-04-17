#include "HeapManager.h"
#include "SkyConsole.h"
#include "kheap.h"

using namespace VirtualMemoryManager;

namespace HeapManager
{
	int m_heapFrameCount = 0;
	
	//Physical Heap Address
	void* m_pKernelHeapPhysicalMemory = 0;

	bool InitKernelHeap(int heapFrameCount)
	{
		PageDirectory* curPageDirectory = GetKernelPageDirectory();

		//힙의 가상주소
		void* pVirtualHeap = (void*)(KERNEL_VIRTUAL_HEAP_ADDRESS);

		//프레임 수만큼 물리 메모리 할당을 요청한다.
		m_heapFrameCount = heapFrameCount;
		m_pKernelHeapPhysicalMemory = PhysicalMemoryManager::AllocBlocks(m_heapFrameCount);
		
		if (m_pKernelHeapPhysicalMemory == NULL)
		{
#ifdef _HEAP_DEBUG
			SkyConsole::Print("kernel heap allocation fail. frame count : %d\n", m_heapFrameCount);
#endif
			
			return false;
		}

		
//페이지 시스템에 힙 가상주소와 물리주소를 매핑힌다.
		MapHeapToAddressSpace(curPageDirectory);

#ifdef _HEAP_DEBUG
		SkyConsole::Print("kernel heap allocation success. frame count : %d\n", m_heapFrameCount);
#endif

		int virtualEndAddress = (uint32_t)pVirtualHeap + m_heapFrameCount * PMM_BLOCK_SIZE;

#ifdef _HEAP_DEBUG
		SkyConsole::Print("Heap Physical Start Address 0x%x\n", m_pKernelHeapPhysicalMemory);
		SkyConsole::Print("Heap Physical End Address 0x%x\n", (int)m_pKernelHeapPhysicalMemory + m_heapFrameCount * PMM_BLOCK_SIZE);

		SkyConsole::Print("Heap Virtual Start Address 0x%x\n", pVirtualHeap);
		SkyConsole::Print("Heap Virtual End Address 0x%x\n", virtualEndAddress);
#endif

		//힙에 할당된 가상 주소 영역을 사용해서 힙 자료구조를 생성한다. 
		create_kernel_heap((u32int)pVirtualHeap, (uint32_t)virtualEndAddress, (uint32_t)virtualEndAddress, 0, 0);

		return true;
	}

	bool MapHeapToAddressSpace(PageDirectory* curPageDirectory)
	{
		
		int endAddress = (uint32_t)KERNEL_VIRTUAL_HEAP_ADDRESS + m_heapFrameCount * PMM_BLOCK_SIZE;
		//int frameCount = (endAddress - KERNEL_VIRTUAL_HEAP_ADDRESS) / PAGE_SIZE;

		for (int i = 0; i < m_heapFrameCount; i++)
		{
			uint32_t virt = (uint32_t)KERNEL_VIRTUAL_HEAP_ADDRESS + i * PAGE_SIZE;
			uint32_t phys = (uint32_t)m_pKernelHeapPhysicalMemory + i * PAGE_SIZE;
			
			MapPhysicalAddressToVirtualAddresss(curPageDirectory, virt, phys, I86_PTE_PRESENT | I86_PTE_WRITABLE);
			
		}

		return true;
	}
}