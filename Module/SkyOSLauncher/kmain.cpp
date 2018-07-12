#include "kmain.h"
#include "ProcessUtil32.h"
#include "ProcessUtil64.h"
#include "PEImage64.h"
#include "memory.h"
#include "Page.h"
#include "SkyStartOption.h"

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

		dd(SKY_CONSOLE_MODE);
		dd(SKY_WIDTH);
		dd(SKY_HEIGHT);
		dd(SKY_BPP)

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

void kmain(unsigned long magic, unsigned long addr)
{
	bool result = false;
	magic += 1;

	SkyConsole::Initialize();
	SkyConsole::Print("32Bit Kernel Loader Entered..\n");

	multiboot_info_t* mb_info = (multiboot_info_t*)addr;

	
	if (strlen(mb_info->cmdline) == 0)
	{
		SkyConsole::Print("Kernel Name Missing. Default Kernel Loading...\n");
		//부트로더를 거쳐가는 경우 매직값을 다르게 한다.
		
		result = Boot32BitMode(magic, mb_info, KERNEL32_NAME);
	}
	else
	{		
		if (strcmp(mb_info->boot_loader_name, "GNU GRUB 0.95") == 0)
		{
			//char* pName = strtok(mb_info->cmdline, " ");
			//pName = strtok(NULL, " ");

			result = Boot32BitMode(magic, mb_info, "ChobitsOS.exe");
		}
		else
		{
			result = Boot32BitMode(magic, mb_info, mb_info->cmdline);
		}
		

		if (result == false) //32비트 커널 실행에 실패하면 64비트로 부팅 시도
		{
			if (Is64BitSwitchPossible() == false)
			{
				SkyConsole::Print("Impossible 64bit Mode\n");
			}

			else
				result = Boot64BitMode(mb_info, mb_info->cmdline);
		}		
	}

	if(result == false)
		SkyConsole::Print("Kernel Load Fail!!\n");

	for (;;);
}

bool Boot32BitMode(unsigned long magic, multiboot_info_t* pBootInfo, char* szKernelName)
{
	Module* pModule = FindModule(pBootInfo, szKernelName);

	if (pModule == nullptr)
	{
		SkyConsole::Print(" %s Kernel Found Fail!!\n", szKernelName);
		for (;;);
	}

	//커널의 이미지 베이스 주소와 커널 엔트리를 찾는다.
	uint32_t kernelEntry = 0;
	uint32_t imageBase = 0;
	kernelEntry = FindKernel32Entry(szKernelName, (char*)pModule->ModuleStart, imageBase);

	if (kernelEntry == 0 || imageBase == 0)
	{
		SkyConsole::Print("Invalid Kernel32 Address!!\n");
		return false;
	}

	uint32_t moduleEndAddress = GetModuleEnd(pBootInfo);

	if(moduleEndAddress > imageBase)
	{
		SkyConsole::Print("Module space and SKYOS32 image base address was overraped. 0x%x 0x%x\n", moduleEndAddress, imageBase);
		SkyConsole::Print("Modify Kernel image base address and check entry point(kmain)\n");
		return false;
	}

	memcpy((void*)imageBase, (void*)pModule->ModuleStart, ((int)pModule->ModuleEnd - (int)pModule->ModuleStart));
	
	__asm
	{
		
		mov		eax, pBootInfo
		push	imageBase
		push	eax
		push	magic
		mov		eax, kernelEntry;
		call	eax;
	}

	return true;
}


bool Boot64BitMode(multiboot_info_t* pBootInfo, char* szKernelName)
{
	Module* pModule = FindModule(pBootInfo, szKernelName);

	if (pModule == nullptr)
	{
		SkyConsole::Print(" %s Kernel Found Fail!!\n", szKernelName);
		for (;;);
	}

	//커널의 이미지 베이스 주소와 커널 엔트리를 찾는다.
	uint32_t kernelEntry = 0;
	uint32_t imageBase = 0;
	kernelEntry = FindKernel64Entry(szKernelName, (char*)pModule->ModuleStart, imageBase);

	if (kernelEntry == 0 || imageBase == 0)
	{
		SkyConsole::Print("Invalid Kernel64 Address!!\n");
		return false;
	}

	//커널 이미지 베이스와 로드된 모듈주소와는 공간이 어느정도 비어 있다고 가정한다.
	//커널64의 이미지베이스 로드 주소는 0x200000이다.

	int pml4EntryAddress = 0x160000;

	//64커널 이미지 베이스 주소에 커널을 카피한다.
	memcpy((void*)imageBase, (void*)pModule->ModuleStart, ((int)pModule->ModuleEnd - (int)pModule->ModuleStart));

	InitializePageTables(pml4EntryAddress);
	SkyConsole::Print("Start %s!!\n", szKernelName);
	SwitchAndExecute64bitKernel(pml4EntryAddress, kernelEntry, (int)pBootInfo);

	return true;
}

bool DetectionCPUID();
bool IsLongModeCheckPossible();
bool IsLongModePossible();

bool Is64BitSwitchPossible()
{
	if (DetectionCPUID() == false)
		return false;

	SkyConsole::Print("CPUID Detected..\n");

	if (IsLongModeCheckPossible() == false)
		return false;

	SkyConsole::Print("Long Mode Check Possible..\n");

	if (IsLongModePossible() == false)
		return false;

	SkyConsole::Print("Long Mode Possible..\n");

	return true;
}


bool DetectionCPUID()
{
	bool result = false;
	__asm
	{
		pushfd
		pop eax

		; Copy to ECX as well for comparing later on
		mov ecx, eax

		; Flip the ID bit
		xor eax, 1 << 21

		; Copy EAX to FLAGS via the stack
		push eax
		popfd

		; Copy FLAGS back to EAX(with the flipped bit if CPUID is supported)
		pushfd
		pop eax

		; Restore FLAGS from the old version stored in ECX(i.e.flipping the ID bit
		; back if it was ever flipped).
		push ecx
		popfd

		; Compare EAX and ECX.If they are equal then that means the bit wasn't
		; flipped, and CPUID isn't supported.
		xor eax, ecx
		jz NoCPUID
		mov result, 1
		NoCPUID:

	}

	return result;
}

bool IsLongModeCheckPossible()
{
	bool result = false;
	__asm
	{
		mov eax, 0x80000000; Set the A - register to 0x80000000.
		cpuid; CPU identification.
		cmp eax, 0x80000001; Compare the A - register with 0x80000001.
		jb NoLongMode; It is less, there is no long mode.
		mov result, 1
		NoLongMode:
	}

	return result;
}

bool IsLongModePossible()
{
	bool result = false;
	__asm
	{
		mov eax, 0x80000001; Set the A - register to 0x80000001.
		cpuid; CPU identification.
		test edx, 1 << 29; Test if the LM - bit, which is bit 29, is set in the D - register.
		jz NoLongMode; They aren't, there is no long mode.
		mov result, 1
		NoLongMode:
	}

	return result;
}

Module* FindModule(multiboot_info_t* pInfo, const char* szFileName)
{	
	uint32_t mb_flags = pInfo->flags;
	void* kentry = nullptr;

	if (mb_flags & MULTIBOOT_INFO_MODS)
	{
		uint32_t mods_count = pInfo->mods_count;
		uint32_t mods_addr = (uint32_t)pInfo->Modules;

		for (uint32_t mod = 0; mod < mods_count; mod++)
		{
			Module* module = (Module*)(mods_addr + (mod * sizeof(Module)));

			const char* module_string = (const char*)module->Name;

			SkyConsole::Print("Module Name : %s\n", module_string);

			if (strcmp(module_string, szFileName) == 0)
			{
				return module;
			}
		}
	}

	return nullptr;
}

uint32_t GetModuleEnd(multiboot_info* bootinfo)
{
	uint32_t endAddress = 0;
	uint32_t mods_count = bootinfo->mods_count;   /* Get the amount of modules available */
	uint32_t mods_addr = (uint32_t)bootinfo->Modules;     /* And the starting address of the modules */
	for (uint32_t i = 0; i < mods_count; i++) {
		Module* module = (Module*)(mods_addr + (i * sizeof(Module)));     /* Loop through all modules */

		uint32_t moduleStart = PAGE_ALIGN_DOWN((uint32_t)module->ModuleStart);
		uint32_t moduleEnd = PAGE_ALIGN_UP((uint32_t)module->ModuleEnd);

		if (endAddress < moduleEnd)
		{
			endAddress = moduleEnd;
		}

		SkyConsole::Print("%s (0x%x - 0x%x)\n", module->Name, moduleStart, moduleEnd);
	}

	return (uint32_t)endAddress;
}
