#include "PIT.h"
#include "Hal.h"
#include "SkyConsole.h"

volatile uint32_t _pitTicks = 0;
int g_esp = 0;
uint32_t g_pageDirectory = 0;
DWORD _lastTickCount = 0;


void ISRHandler(registers_t regs)
{
	//SwitchTask(_pitTicks, regs);
}

//타이머 인터럽트 핸들러
__declspec(naked) void InterruptPITHandler() 
{	
	_asm
	{
		cli
		pushad;

		push ds
			push es
			push fs
			push gs

			mov ax, 0x10; load the kernel data segment descriptor
			mov ds, ax
			mov es, ax
			mov fs, ax
			mov gs, ax

			mov eax, esp
			mov g_esp, eax
	}
	_pitTicks++;

	_asm
	{
		call ISRHandler
	}

	__asm
	{
		cmp g_pageDirectory, 0
		jz pass

		mov eax, g_esp
		mov esp, eax

		mov	eax, [g_pageDirectory]
		mov	cr3, eax		// PDBR is cr3 register in i86

		pop gs
		pop fs
		pop es
		pop ds

		popad;

		mov al, 0x20
			out 0x20, al
			sti
			iretd;

	pass:
		pop gs
			pop fs
			pop es
			pop ds

			popad;
		mov al, 0x20
			out 0x20, al
			sti
			iretd;
	}
}

//타이머를 시작
void StartPITCounter(uint32_t freq, uint8_t counter, uint8_t mode) {

	if (freq == 0)
		return;

	uint16_t divisor = uint16_t(1193181 / (uint16_t)freq);

	//커맨드 전송
	uint8_t ocw = 0;
	ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
	ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
	SendPITCommand(ocw);

	//프리퀀시 비율 설정
	SendPITData(divisor & 0xff, 0);
	SendPITData((divisor >> 8) & 0xff, 0);

	//타이머 틱 카운트 리셋
	_pitTicks = 0;
}

//PIT 초기화
void InitializePIT()
{
	setvect(32, InterruptPITHandler);
}

uint32_t SetPITTickCount(uint32_t i) {

	uint32_t ret = _pitTicks;
	_pitTicks = i;
	return ret;
}


//! returns current tick count
uint32_t GetPITTickCount() {

	return _pitTicks;
}

unsigned int GetTickCount()
{
	return _pitTicks;
}

void _cdecl msleep(int ms)
{

	unsigned int ticks = ms + GetTickCount();
	while (ticks > GetTickCount())
		;
}

void SendPITCommand(uint8_t cmd) {

	OutPortByte(I86_PIT_REG_COMMAND, cmd);
}

void SendPITData(uint16_t data, uint8_t counter) {

	uint8_t	port = (counter == I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
		((counter == I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

	OutPortByte(port, (uint8_t)data);
}