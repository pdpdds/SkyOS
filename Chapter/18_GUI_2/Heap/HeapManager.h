#pragma once
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"

namespace HeapManager
{
	bool InitKernelHeap(int heapFrameCount);
	bool MapHeapToAddressSpace(PageDirectory* curPageDirectory);
	DWORD GetHeapSize();
	DWORD GetUsedHeapSize();
}