#include "kmain.h"


/*

GRUB 가 kernel 을 호풀할 준비가 됐지만, kernel entry point 즉 kernel 함수 시작 지점을 호출하기 위해 kernel 은 특정 명세를 준수

GRUB 는 kernel 이 특정 명세를 준수하고 있는지의 여부를 다음과 같은 mechanism 으로 파악
> Kernel file 에서 최초 80KB 부분을 검색해서 특정 Signature 를 찾아냄

그렇기 때문에 특정 signature 파일을 80KB 부분을 검색해서 찾아냄
이 Signature 를 Multi boot header structure 라고 부름

*/

/*

GRUB 는 MULTIBOOT_HEADER 구조체에 정의된
MULTIBOOT_HEADER_MAGIC Signature 를 찾고
이 값이 자신이 정의한 값과 같은지 확인

#define MULTIBOOT_HEADER_MAGIC         0x1BADB002

GRUB 는 Signature 를 찾은 후 Kernel 임을 확정짓고
Multiboot header값 중 entry address 를 담은 member value를 읽어와
해당 address 로 jump

*/


// naked : stack frame 을 형성하지 않고, 어떤 코드도 직접 호출하지 않음
_declspec(naked) void multiboot_entry(void)
{
	__asm {

		// dd = define double word 를 의미하는 macro
		// 4byte 데이터 공간 할당

		align 4 // 4 byte 정렬

		multiboot_header:

		//멀티부트 헤더 사이즈 : 0X20
		dd(MULTIBOOT_HEADER_MAGIC); magic number
		dd(MULTIBOOT_HEADER_FLAGS); flags
		dd(CHECKSUM); checksum
		dd(HEADER_ADRESS); //헤더 주소 KERNEL_LOAD_ADDRESS+ALIGN(0x100064)
		dd(KERNEL_LOAD_ADDRESS); //커널이 로드된 가상주소 공간
		dd(00); //사용되지 않음
		dd(00); //사용되지 않음
		dd(HEADER_ADRESS + 0x20); 
		//커널 시작 주소 : 멀티부트 헤더 주소 + 0x20, kernel_entry

		/*

		이 부분은 실제 Kernel code 가 실행
		Assembly code 에서는 kernel stack 을 초기화하고
		Multiboot 정보를 담은 구조체의 address 와 magic number를 stack 에 담은 후
		실제 kernel entry 인 kmain 호출

		*/

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

void InitializeConstructor()
{
	//내부 구현은 나중에 추가한다.
}

void kmain(unsigned long magic, unsigned long addr)
{
	InitializeConstructor(); //글로벌 객체 초기화

	/*
	
	SkeyConsole : Console 에 log 를 남길 수 있는 개체
	Console screen 에 문자열 출력을 가능하게 해주는 Console logger

	*/

	SkyConsole::Initialize(); //화면에 문자열을 찍기 위해 초기화한다.

	SkyConsole::Print("Hello World!!\n");

	for (;;); //메인함수의 진행을 막음, 루프
}