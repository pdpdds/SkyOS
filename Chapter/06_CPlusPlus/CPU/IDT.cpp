#include "idt.h"
#include "string.h"
#include "memory.h"
#include <hal.h>
#include "SkyAPI.h"

//인터럽트 디스크립터 테이블
static struct idt_descriptor	_idt [I86_MAX_INTERRUPTS];

//CPU의 IDTR 레지스터를 초기화하는데 도움을 주는 IDTR 구조체
static struct idtr				_idtr;

//IDTR 레지스터에 IDT의 주소값을 넣는다.
static void IDTInstall() {
#ifdef _MSC_VER
	_asm lidt [_idtr]
#endif
}

//다룰수 있는 핸들러가 존재하지 않을때 호출되는 기본 핸들러
__declspec(naked) void InterrputDefaultHandler () {
	
	_asm {
		cli
		pushad
	}

	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		sti
		iretd
	}
}

//i번째 인터럽트 디스크립트를 얻어온다.
idt_descriptor* GetInterruptDescriptor(uint32_t i) {

	if (i>I86_MAX_INTERRUPTS)
		return 0;

	return &_idt[i];
}

//인터럽트 핸들러 설치
bool InstallInterrputHandler(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER irq) {

	if (i>I86_MAX_INTERRUPTS)
		return false;

	if (!irq)
		return false;

	//인터럽트의 베이스 주소를 얻어온다.
	uint64_t		uiBase = (uint64_t)&(*irq);
	
	if ((flags & 0x0500) == 0x0500) {
		_idt[i].sel = sel;
		_idt[i].flags = uint8_t(flags);
	}
	else
	{
		//포맷에 맞게 인터럽트 핸들러와 플래그 값을 디스크립터에 세팅한다.
		_idt[i].baseLo = uint16_t(uiBase & 0xffff);
		_idt[i].baseHi = uint16_t((uiBase >> 16) & 0xffff);
		_idt[i].reserved = 0;
		_idt[i].flags = uint8_t(flags);
		_idt[i].sel = sel;
	}

	return	true;
}

//IDT를 초기화하고 디폴트 핸들러를 등록한다
bool IDTInitialize(uint16_t codeSel) {

	//IDTR 레지스터에 로드될 구조체 초기화
	_idtr.limit = sizeof(struct idt_descriptor) * I86_MAX_INTERRUPTS - 1;
	_idtr.base = (uint32_t)&_idt[0];

	//NULL 디스크립터
	memset((void*)&_idt[0], 0, sizeof(idt_descriptor) * I86_MAX_INTERRUPTS - 1);

	//디폴트 핸들러 등록
	for (int i = 0; i<I86_MAX_INTERRUPTS; i++)
		InstallInterrputHandler(i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)InterrputDefaultHandler);

	//IDTR 레지스터를 셋업한다
	IDTInstall();

	return true;
}


