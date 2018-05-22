#include "kmain.h"
#include "SkyTest.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"
#include "Exception.h"
#include "FPU.h"

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
		mov     esp, 0x40000; //스택 설정

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

void HardwareInitialize();
bool InitMemoryManager(multiboot_info* bootinfo);

void kmain(unsigned long magic, unsigned long addr)
{				
	InitializeConstructors();

	multiboot_info* pBootInfo = (multiboot_info*)addr;

	SkyConsole::Initialize();
		
	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.	
	SkyConsole::Print("*** Sky OS Console System Init ***\n");

	SkyConsole::Print("GRUB Information\n");
	SkyConsole::Print("Boot Loader Name : %s\n", (char*)pBootInfo->boot_loader_name);

	kEnterCriticalSection();

	HardwareInitialize();
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
	
//물리/가상 메모리 매니저를 초기화한다.
	InitMemoryManager(pBootInfo);

	SkyConsole::Print("Memory Manager Init Complete\n");
	
	kLeaveCriticalSection();

	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);		

	for (;;);
}

void HardwareInitialize()
{
	GDTInitialize();
	IDTInitialize(0x8);
	PICInitialize(0x20, 0x28);
	InitializePIT();
}

bool InitMemoryManager(multiboot_info* pBootInfo)
{
	//물리/가상 메모리 매니저를 초기화한다.
	//기본 설정 시스템 메모리는 128MB
	SkyConsole::Print("Memory Manager Init Complete\n");

	PhysicalMemoryManager::EnablePaging(false);

	//물리 메모리 매니저 초기화
	PhysicalMemoryManager::Initialize(pBootInfo);
	PhysicalMemoryManager::Dump();
	
	//가상 메모리 매니저 초기화	
	VirtualMemoryManager::Initialize();
	PhysicalMemoryManager::Dump();

	return true;
}