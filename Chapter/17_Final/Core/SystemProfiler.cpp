#include "SkyOS.h"
#include "PCI.h"
#include "SkyPCI.h"

SystemProfiler* SystemProfiler::m_pSystemProfiler = nullptr;
extern PageDirectory* g_pageDirectoryPool[MAX_PAGE_DIRECTORY_COUNT];
extern uint32_t g_kernel_load_address;
extern int g_stackPhysicalAddressPool;

SystemProfiler::SystemProfiler()
{
}

SystemProfiler::~SystemProfiler()
{
}

bool SystemProfiler::Initialize()
{
	UINT16 pciDevices = InitPCIDevices();
	SkyConsole::Print("%d device(s) found\n", pciDevices);

	ScanPCIDevices();

	GlobalSate state;
	state._HeapLoadAddress = KERNEL_VIRTUAL_HEAP_ADDRESS;
	state._heapSize = HeapManager::GetHeapSize();
	state._kernelLoadAddress = g_kernel_load_address;
	
	state._kernelSize = PhysicalMemoryManager::GetKernelSize();

	state._stackPhysicalPoolAddress = g_stackPhysicalAddressPool;
	state._pciDevices = pciDevices;
	state._pageDirectoryPoolAddress = (DWORD)&(g_pageDirectoryPool[0]);

	SystemProfiler::GetInstance()->SetGlobalState(state);

	return true;
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

	SkyConsole::Print("Heap Size : %d(MB)\n", heapSize / MEGA_BYTES);
	SkyConsole::Print("Used Heap Size : %d(Bytes)\n", usedHeapSize);
}

void SystemProfiler::PrintGlobalState()
{
	SkyConsole::Print("Kernel Load Address : 0x%x\n", m_state._kernelLoadAddress);
	SkyConsole::Print("Kernel Size : %d(KB)\n", m_state._kernelSize/ KILO_BYTES);

	SkyConsole::Print("Heap Start Address : 0x%x\n", m_state._HeapLoadAddress);
	SkyConsole::Print("Heap Size : %d(MB)\n", m_state._heapSize / MEGA_BYTES);

	SkyConsole::Print("Stack Physical Pool Address : 0x%x\n", m_state._stackPhysicalPoolAddress);
	SkyConsole::Print("Page Directory Pool Address  : 0x%x\n", m_state._pageDirectoryPoolAddress);
}