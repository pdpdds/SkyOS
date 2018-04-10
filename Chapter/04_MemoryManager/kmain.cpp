#include "kmain.h"
#include "SkyTest.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"

_declspec(naked) void multiboot_entry(void)
{
	__asm {
		align 4

		multiboot_header:
		//멀티부트 헤더 사이즈 : 0X20
		dd(MULTIBOOT_HEADER_MAGIC); magic number
		dd(MULTIBOOT_HEADER_FLAGS); flags
		dd(CHECKSUM); checksum
		dd(HEADER_ADRESS); //헤더 주소 KERNEL_LOAD_ADDRESS+ALIGN(0x100064)
		dd(KERNEL_LOAD_ADDRESS); //커널이 로드된 가상주소 공간
		dd(00); //사용되지 않음
		dd(00); //사용되지 않음
		dd(HEADER_ADRESS + 0x20); //커널 시작 주소 : 멀티부트 헤더 주소 + 0x20, kernel_entry
			
		kernel_entry :
		mov     esp, KERNEL_STACK; //스택 설정

		push    0; //플래그 레지스터 초기화
		popf

		//GRUB에 의해 담겨 있는 정보값을 스택에 푸쉬한다.
		push    ebx; //멀티부트 구조체 포인터
		push    eax; //매직 넘버

		//위의 두 파라메터와 함께 kmain 함수를 호출한다.
		call    kmain; //C++ 메인 함수 호출

		//루프를 돈다. kmain이 리턴되지 않으면 아래 코드는 수행되지 않는다.
		halt:
		jmp halt;
	}
}

void HardwareInitiize();
bool InitMemoryManager(multiboot_info* bootinfo);

void kmain(unsigned long magic, unsigned long addr)
{				
	InitializeConstructors();

	multiboot_info* pBootInfo = (multiboot_info*)addr;

	SkyConsole::Initialize();
		
	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.	
	SkyConsole::Print("*** Sky OS Console System Init ***\n");

	kEnterCriticalSection(&g_criticalSection);

	HardwareInitiize();
	SkyConsole::Print("Hardware Init Complete\n");

	SetInterruptVector();
	
	SkyConsole::Print("Interrput Handler Init Complete\n");

//물리/가상 메모리 매니저를 초기화한다.
	InitMemoryManager(pBootInfo);

	SkyConsole::Print("Memory Manager Init Complete\n");

	kLeaveCriticalSection(&g_criticalSection);

	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);		

	for (;;);
}

void HardwareInitiize()
{
	GDTInitialize();
	IDTInitialize(0x8);
	PICInitialize(0x20, 0x28);
	InitializePIT();
}

uint32_t GetFreeSpaceMemory(multiboot_info* bootinfo)
{
	uint32_t memorySize = 0;
	uint32_t mmapEntryNum = bootinfo->mmap_length / sizeof(multiboot_memory_map_t);

	//SkyConsole::Print("GRUB Information\n");
	//SkyConsole::Print("Boot Loader Name : %s\n", (char*)bootinfo->boot_loader_name);
	//SkyConsole::Print("Memory Map Entry Num : %d\n", mmapEntryNum);

	uint32_t mmapAddr = bootinfo->mmap_addr;

	for (uint32_t i = 0; i < mmapEntryNum; i++)
	{
		multiboot_memory_map_t* entry = (multiboot_memory_map_t*)mmapAddr;

#ifdef _SKY_DEBUG
		SkyConsole::Print("Memory Address : %x\n", entry->addr);
		SkyConsole::Print("\nMemory Length : %x", entry->len);
		SkyConsole::Print("\nMemory Type : %x", entry->type);
		SkyConsole::Print("\nEntry Size : %d", entry->size);
#endif

		mmapAddr += sizeof(multiboot_memory_map_t);

		if (entry->addr + entry->len < FREE_MEMORY_SPACE_ADDRESS)
			continue;

		memorySize += entry->len;
		if (entry->addr < FREE_MEMORY_SPACE_ADDRESS)
			memorySize -= (FREE_MEMORY_SPACE_ADDRESS - entry->addr);
	}

	memorySize -= (memorySize % 4096);

	return memorySize;
}

bool InitMemoryManager(multiboot_info* bootinfo)
{
	PhysicalMemoryManager::EnablePaging(false);

	uint32_t freeSpaceMemorySize = GetFreeSpaceMemory(bootinfo);

	//freeSpaceMemorySize = 4096 * 1024 * 200;

	//SkyConsole::Print("KernelSize : %d Bytes\n", kernelSize);
	SkyConsole::Print("FreeSpace MemorySize From 0x%x: 0x%x Bytes\n", FREE_MEMORY_SPACE_ADDRESS, freeSpaceMemorySize);


	//물리 메모리 매니저 초기화
	PhysicalMemoryManager::Initialize(freeSpaceMemorySize, FREE_MEMORY_SPACE_ADDRESS);
	//PhysicalMemoryManager::Dump();

	uint32_t memoryMapSize = PhysicalMemoryManager::GetMemoryMapSize();
	uint32_t alignedMemoryMapSize = (memoryMapSize / 4096) * 4096;

	if (memoryMapSize % 4096 > 0)
		alignedMemoryMapSize += 4096;

	PhysicalMemoryManager::SetAvailableMemory(FREE_MEMORY_SPACE_ADDRESS + alignedMemoryMapSize, freeSpaceMemorySize - (FREE_MEMORY_SPACE_ADDRESS + alignedMemoryMapSize));
	PhysicalMemoryManager::Dump();

	//가상 메모리 매니저 초기화	
	VirtualMemoryManager::Initialize();

	SkyConsole::Print("Init Complete\n");
	
	return true;
}