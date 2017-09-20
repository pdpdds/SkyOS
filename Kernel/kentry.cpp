#include "SkyConsole.h"
#include "stdint.h"
#include "MultiBoot.h"

uint32_t g_kernelSize = 0;

//글로벌, 정적 오브젝트 초기화를 위해 구현해야 되는 함수
extern void __cdecl  InitializeConstructors();
extern void __cdecl  Exit();

extern void kmain(unsigned long magic, unsigned long addr);

//부트로더에 의해 호출되는 커널 엔트리
//현재는 GRUB이 부트로더 역할을 하고 있는데
//kmain 함수를 직접 호출하고 있으므로 kernel_entry는 사용되지 않는다
void __cdecl  kernel_entry(multiboot_info* bootinfo) 
{
	//보호모드를 위해 레지스터 초기화
	_asm 
	{
		cli
		mov ax, 10h
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
	}

	//dx 레지스터에는 커널의 크기가 담겨 있다.
	//다른값으로 씌워지기 전에 값을 얻어낸다.
	_asm	mov	word ptr[g_kernelSize], dx
	InitializeConstructors();
	kmain(MULTIBOOT_HEADER_MAGIC, (unsigned long)bootinfo);
	Exit();

	_asm 
	{
		cli
		hlt
	}

}

