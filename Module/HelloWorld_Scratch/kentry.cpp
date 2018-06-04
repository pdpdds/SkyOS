#include "SkyConsole.h"
#include "stdint.h"
#include "MultiBoot.h"

uint32_t g_kernelSize = 0;

//글로벌, 정적 오브젝트 초기화를 위해 구현해야 되는 함수
extern void __cdecl  InitializeConstructors();
extern void __cdecl  Exit();

extern void kmain(unsigned long magic, unsigned long addr);

//부트로더에 의해 호출되는 커널 엔트리
//GRUB이 multiboot_entry => kmain 순으로 호출한다면
//여기서는 kernel_entry => kmain 순으로 호출된다.
//GRUB의 부팅과 호환성을 맞추기 위해 어셈블리 코드에서도 multiboot_info 구조체 정보를 넘긴다.
//하지만 정보는 대부분 채워져 있지 않다.
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

