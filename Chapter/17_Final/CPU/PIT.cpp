#include "PIT.h"
#include "Hal.h"
#include "SkyConsole.h"
#include "Scheduler.h"
#include "ProcessManager.h"
#include "Process.h"
#include "Thread.h"

extern void SwitchTask(int tick, registers_t& regs);
extern void SendEOI();

volatile uint32_t _pitTicks = 0;
int g_esp = 0;
uint32_t g_pageDirectory = 0;
DWORD _lastTickCount = 0;


void ISRHandler(registers_t regs)
{
	SwitchTask(_pitTicks, regs);
}

__declspec(naked) void kDefaultInterruptHandler()
{
	_asm {
		cli
		pushad
	}

	if (_pitTicks - _lastTickCount >= 100)
	{
		_lastTickCount = _pitTicks;
		SkyConsole::Print("Timer Count : %d\n", _pitTicks);
	}

	_pitTicks++;

	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		sti
		iretd
	}
}

//타이머 인터럽트 핸들러
__declspec(naked) void InterruptPITHandler() 
{	
	_asm
	{
		//모든 레지스터를 스택에 넣는다.
		PUSHFD
		cli
		pushad;
		push ds
		push es
		push fs
		push gs

		mov ax, 0x10; 커널 데이터 세그먼트 셀렉터 선택
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax

		mov eax, esp; 현재 ESP값을 저장한다.
		mov g_esp, eax
	}

	_pitTicks++;

	_asm
	{
		call ISRHandler; 타이머 인터럽트를 처리한다.
	}

	__asm
	{
		cmp g_pageDirectory, 0; 페이지 디렉토리 값이 0이면 정상적으로 인터럽트를 완료
		jz pass

		//페이지 디렉토리값이 설정되어 있다면 
		//스택 포인터와 페이지 디렉토리를 변경해서 컨택스트 스위칭을 수행한다.
		mov eax, g_esp
		mov esp, eax

		mov	eax, [g_pageDirectory]
		mov	cr3, eax; CR3(PDBR) 레지스터에 페이지 디렉토리값 변경
		pass :
		//스택에 넣었던 레지스터 값들을 복원하고 원래 수행하던 코드로 리턴한다
		pop gs
			pop fs
			pop es
			pop ds

			popad;
	}
	SendEOI();

	__asm
	{
		
		POPFD
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
void PITInitialize()
{
	SetInterruptVector(32, InterruptPITHandler);
}

unsigned int GetTickCount()
{
	return _pitTicks;
}

void msleep(int ms)
{
	unsigned int ticks = ms + GetTickCount();
	while (ticks >= GetTickCount())
	{				
		Scheduler::GetInstance()->Yield();
	}
}

void SendPITCommand(uint8_t cmd) {

	OutPortByte(I86_PIT_REG_COMMAND, cmd);
}

void SendPITData(uint16_t data, uint8_t counter) {

	uint8_t	port = (counter == I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
		((counter == I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

	OutPortByte(port, (uint8_t)data);
}