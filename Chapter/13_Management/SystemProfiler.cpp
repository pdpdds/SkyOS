#include "SystemProfiler.h"
#include "SkyConsole.h"
#include "PhysicalMemoryManager.h"
#include "HeapManager.h"

#define MEGA_BYTES 1048576
#define KILO_BYTES 1024

SystemProfiler* SystemProfiler::m_pSystemProfiler = nullptr;

SystemProfiler::SystemProfiler()
{
}


SystemProfiler::~SystemProfiler()
{
}

void SystemProfiler::PrintMemoryState()
{
	int memorySize = PhysicalMemoryManager::GetMemorySize();
	int freeSize = PhysicalMemoryManager::GetFreeMemory();
	int heapSize = HeapManager::GetHeapSize();
	int usedHeapSize = HeapManager::GetUsedHeapSize();

	SkyConsole::Print("Total Memory : %d(MB)\n", memorySize / MEGA_BYTES);
	SkyConsole::Print("Available Memory : %d(MB)\n", freeSize / MEGA_BYTES);

	SkyConsole::Print("Total Block Count %d\n", PhysicalMemoryManager::GetTotalBlockCount());
	SkyConsole::Print("Free Block Count %d\n", PhysicalMemoryManager::GetFreeBlockCount());

	SkyConsole::Print("\nHeap Size : %d(MB)\n", heapSize / MEGA_BYTES);
	SkyConsole::Print("\nUsed Heap Size : %d(Bytes)\n", usedHeapSize);
}

void SystemProfiler::PrintGlobalState()
{
	SkyConsole::Print("Kernel Load Address : 0x%x\n", m_state._kernelLoadAddress);
	SkyConsole::Print("Kernel Size : %d(MB)\n", m_state._kernelSize/ MEGA_BYTES);

	SkyConsole::Print("Heap Start Address : 0x%x\n", m_state._HeapLoadAddress);
	SkyConsole::Print("Heap Size : %d(MB)\n", m_state._heapSize / MEGA_BYTES);

	SkyConsole::Print("Stack Physical Pool Address : 0x%x\n", m_state._stackPhysicalPoolAddress);
	SkyConsole::Print("Page Directory Pool Address  : 0x%x\n", m_state._pageDirectoryPoolAddress);
}