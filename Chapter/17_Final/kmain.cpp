#include "kmain.h"
#include "SkyGUILauncher.h"
#include "SkyConsoleLauncher.h"
#include "SkyTest.h"

uint32_t g_kernel_load_address = 0;

void kmain(unsigned long magic, unsigned long addr, uint32_t imageBase)
{
#ifdef SKY_EMULATOR
	kmainSkyEmulator();
	return;
#endif
	multiboot_info* pBootInfo = (multiboot_info*)addr;
	SkyLauncher* pSystemLauncher = nullptr;
	
	//매직값이 다르다면 이 커널은 GRUB => 부트로더에 의해 로드되었다고 판단	
	//베이스 어드레스 0x400000
	//메인엔트리 kmain
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{	
		g_kernel_load_address = imageBase;
	}
	else
	{
		//GRUB에 단독으로 부팅되었을 경우
		//베이스 어드레스 0x100000
		//메인엔트리 multiboot_entry	
		g_kernel_load_address = GRUB_KERNEL_LOAD_ADDRESS;		
	}

	InitContext(pBootInfo);
	
	kEnterCriticalSection();
	
	InitHardware();
	InitMemoryManager(pBootInfo);

	Scheduler::GetInstance();
	
	SkyModuleManager::GetInstance()->Initialize(pBootInfo);
	SystemProfiler::GetInstance()->Initialize();

#if SKY_CONSOLE_MODE == 0
	SkyGUISystem::GetInstance()->Initialize(pBootInfo);
#endif	
	
	PrintCurrentTime();
	
	kLeaveCriticalSection();

#if SKY_CONSOLE_MODE == 0	
	pSystemLauncher = new SkyGUILauncher();
#else
	pSystemLauncher = new SkyConsoleLauncher();	
#endif

	pSystemLauncher->Launch();

	for (;;);	
}

void InitContext(multiboot_info* pBootInfo)
{
	InitializeConstructors();
	SkyConsole::Initialize();
	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.	
	SkyConsole::Print("*** Sky OS Console System Init ***\n");
	SkyConsole::Print("GRUB Information\n");
	SkyConsole::Print("Boot Loader Name : %s\n", (char*)pBootInfo->boot_loader_name);
}

void InitHardware()
{
	GDTInitialize();
	IDTInitialize(0x8);
	PICInitialize(0x20, 0x28);
	PITInitialize();

	SkyConsole::Print("Hardware Init Complete\n");

	SetInterruptVector();
	InitializeSysCall();
	//현재 구조에서는 TSS가 필요없음
	//InstallTSS(5, 0x10, 0);

	SkyConsole::Print("Interrput Handler Init Complete\n");

	if (false == InitFPU())
	{
		SkyConsole::Print("[Warning] Floating Pointer Unit Detection Fail\n");
	}
	else
	{
		EnableFPU();
		SkyConsole::Print("FPU Init..\n");
	}
}

bool InitMemoryManager(multiboot_info* pBootInfo)
{
	//물리/가상 메모리 매니저를 초기화한다.
	//기본 설정 시스템 메모리는 128MB
	SkyConsole::Print("Memory Manager Init Complete\n");

	PhysicalMemoryManager::EnablePaging(false);

	//물리 메모리 매니저 초기화
	PhysicalMemoryManager::Initialize(pBootInfo);
	//PhysicalMemoryManager::Dump();

	//가상 메모리 매니저 초기화	
	VirtualMemoryManager::Initialize();
	//PhysicalMemoryManager::Dump();

	int heapFrameCount = 256 * 10 * 5; //프레임수 12800개, 52MB
	unsigned int requiredHeapSize = heapFrameCount * PAGE_SIZE;
	
	//요구되는 힙의 크기가 자유공간보다 크다면 그 크기를 자유공간 크기로 맞춘다음 반으로 줄인다.
	uint32_t memorySize = PhysicalMemoryManager::GetMemorySize();
	if (requiredHeapSize > memorySize)
	{
		requiredHeapSize = memorySize / 2;
		heapFrameCount = requiredHeapSize / PAGE_SIZE / 2;
	}
	
	HeapManager::InitKernelHeap(heapFrameCount);
	SkyConsole::Print("Heap %dMB Allocated\n", requiredHeapSize / MEGA_BYTES);
	
	return true;
}

void SetInterruptVector()
{
	SetInterruptVector(0, (void(__cdecl &)(void))kHandleDivideByZero);
	SetInterruptVector(1, (void(__cdecl &)(void))kHandleSingleStepTrap);
	SetInterruptVector(2, (void(__cdecl &)(void))kHandleNMITrap);
	SetInterruptVector(3, (void(__cdecl &)(void))kHandleBreakPointTrap);
	SetInterruptVector(4, (void(__cdecl &)(void))kHandleOverflowTrap);
	SetInterruptVector(5, (void(__cdecl &)(void))kHandleBoundsCheckFault);
	SetInterruptVector(6, (void(__cdecl &)(void))kHandleInvalidOpcodeFault);
	SetInterruptVector(7, (void(__cdecl &)(void))kHandleNoDeviceFault);
	SetInterruptVector(8, (void(__cdecl &)(void))kHandleDoubleFaultAbort);
	SetInterruptVector(10, (void(__cdecl &)(void))kHandleInvalidTSSFault);
	SetInterruptVector(11, (void(__cdecl &)(void))kHandleSegmentFault);
	SetInterruptVector(12, (void(__cdecl &)(void))kHandleStackFault);
	SetInterruptVector(13, (void(__cdecl &)(void))kHandleGeneralProtectionFault);
	SetInterruptVector(14, (void(__cdecl &)(void))kHandlePageFault);
	SetInterruptVector(16, (void(__cdecl &)(void))kHandlefpu_fault);
	SetInterruptVector(17, (void(__cdecl &)(void))kHandleAlignedCheckFault);
	SetInterruptVector(18, (void(__cdecl &)(void))kHandleMachineCheckAbort);
	SetInterruptVector(19, (void(__cdecl &)(void))kHandleSIMDFPUFault);

	SetInterruptVector(33, (void(__cdecl &)(void))InterrputDefaultHandler);
	SetInterruptVector(38, (void(__cdecl &)(void))InterrputDefaultHandler);
}

_declspec(naked) void multiboot_entry(void)
{
#ifdef SKY_EMULATOR
	__asm
	{
		MOV EAX, 0
		PUSH EAX;
		PUSH EAX;
		PUSH EAX;
		CALL    kmain
	}
#else
	__asm {
		align 4

		multiboot_header:
		//멀티부트 헤더 사이즈 : 0X30
		dd(MULTIBOOT_HEADER_MAGIC); magic number

#if SKY_CONSOLE_MODE == 0
		dd(MULTIBOOT_HEADER_FLAGS_GUI); flags
		dd(CHECKSUM_GUI); checksum
#else
		dd(MULTIBOOT_HEADER_FLAGS); flags
		dd(CHECKSUM); checksum
#endif		
		dd(HEADER_ADRESS); //헤더 주소 KERNEL_LOAD_ADDRESS+ALIGN(0x100400)
		dd(GRUB_KERNEL_LOAD_ADDRESS); //커널이 로드된 주소
		dd(00); //사용되지 않음
		dd(00); //사용되지 않음
		dd(HEADER_ADRESS + 0x30); //커널 시작 주소 : 멀티부트 헤더 주소 + 0x30, kernel_entry

		dd(SKY_CONSOLE_MODE);
		dd(SKY_WIDTH);
		dd(SKY_HEIGHT);
		dd(SKY_BPP)

	kernel_entry:
		MOV     ESP, 0x40000; //스택 설정

		PUSH    0; //플래그 레지스터 초기화
		POPF

		PUSH    GRUB_KERNEL_LOAD_ADDRESS;
		//GRUB에 의해 담겨 있는 정보값을 스택에 푸쉬한다.
		PUSH    EBX; //멀티부트 구조체 포인터
		PUSH    EAX; //매직 넘버

					 //위의 두 파라메터와 함께 kmain 함수를 호출한다.
		CALL    kmain; //C++ 메인 함수 호출

	//루프를 돈다. kmain이 리턴되지 않으면 아래 코드는 수행되지 않는다.
	halt:
		jmp halt;
	}
#endif
}

void kmainSkyEmulator()
{

}