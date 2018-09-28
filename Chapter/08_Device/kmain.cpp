#include "kmain.h"
#include "SkyTest.h"

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack);
DWORD WINAPI SystemConsoleProc(LPVOID parameter);

_declspec(naked) void multiboot_entry(void)
{
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
		dd(KERNEL_LOAD_ADDRESS); //커널이 로드된 가상주소 공간
		dd(00); //사용되지 않음
		dd(00); //사용되지 않음
		dd(HEADER_ADRESS + 0x30); //커널 시작 주소 : 멀티부트 헤더 주소 + 0x30, kernel_entry

		dd(1);
		dd(0);
		dd(0);
		dd(0)

	kernel_entry:
		MOV     ESP, 0x40000; //스택 설정

		PUSH    0; //플래그 레지스터 초기화
		POPF

		//GRUB에 의해 담겨 있는 정보값을 스택에 푸쉬한다.
		PUSH    EBX; //멀티부트 구조체 포인터
		PUSH    EAX; //매직 넘버

		//위의 두 파라메터와 함께 kmain 함수를 호출한다.
		CALL    kmain; //C++ 메인 함수 호출

		//루프를 돈다. kmain이 리턴되지 않으면 아래 코드는 수행되지 않는다.
	halt:
		jmp halt;
	}
}

void InitContext(multiboot_info* bootinfo);
void InitHardware();
bool InitMemoryManager(multiboot_info* bootinfo);

void kmain(unsigned long magic, unsigned long addr)
{
	multiboot_info* pBootInfo = (multiboot_info*)addr;

	InitContext(pBootInfo);

	InitHardware();
	InitMemoryManager(pBootInfo);
	
	SkyModuleManager::GetInstance()->Initialize(pBootInfo);
	
	JumpToNewKernelEntry((int)SystemConsoleProc, 0x40000);
	for (;;);
}

DWORD WINAPI SystemConsoleProc(LPVOID parameter)
{
	SkyConsole::Print("Console Mode Start!!\n");	
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	
	multiboot_info* pBootInfo = SkyModuleManager::GetInstance()->GetMultiBootInfo();
	StorageManager::GetInstance()->Initilaize(pBootInfo);
	
	for (;;);

	return 0;
}

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack)
{
	__asm
	{
		MOV     AX, 0x10;
		MOV     DS, AX
		MOV     ES, AX
		MOV     FS, AX
		MOV     GS, AX

		MOV     ESP, procStack
		PUSH	0; //파라메터
		PUSH	0; //EBP
		PUSH    0x200; EFLAGS
		PUSH    0x08; CS
		PUSH    entryPoint; EIP
		IRETD
	}
}

void InitContext(multiboot_info* pBootInfo)
{
	//InitializeConstructors();
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
	SkyConsole::Print("Heap %dMB Allocated\n", requiredHeapSize / 1048576);

	return true;
}

void SetInterruptVector()
{
	setvect(0, (void(__cdecl &)(void))kHandleDivideByZero);
	setvect(1, (void(__cdecl &)(void))kHandleSingleStepTrap);
	setvect(2, (void(__cdecl &)(void))kHandleNMITrap);
	setvect(3, (void(__cdecl &)(void))kHandleBreakPointTrap);
	setvect(4, (void(__cdecl &)(void))kHandleOverflowTrap);
	setvect(5, (void(__cdecl &)(void))kHandleBoundsCheckFault);
	setvect(6, (void(__cdecl &)(void))kHandleInvalidOpcodeFault);
	setvect(7, (void(__cdecl &)(void))kHandleNoDeviceFault);
	setvect(8, (void(__cdecl &)(void))kHandleDoubleFaultAbort);
	setvect(10, (void(__cdecl &)(void))kHandleInvalidTSSFault);
	setvect(11, (void(__cdecl &)(void))kHandleSegmentFault);
	setvect(12, (void(__cdecl &)(void))kHandleStackFault);
	setvect(13, (void(__cdecl &)(void))kHandleGeneralProtectionFault);
	setvect(14, (void(__cdecl &)(void))kHandlePageFault);
	setvect(16, (void(__cdecl &)(void))kHandlefpu_fault);
	setvect(17, (void(__cdecl &)(void))kHandleAlignedCheckFault);
	setvect(18, (void(__cdecl &)(void))kHandleMachineCheckAbort);
	setvect(19, (void(__cdecl &)(void))kHandleSIMDFPUFault);

	setvect(33, (void(__cdecl &)(void))InterrputDefaultHandler);
	setvect(38, (void(__cdecl &)(void))InterrputDefaultHandler);
}