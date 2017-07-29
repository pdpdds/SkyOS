#include "PIT.h"
#include "Hal.h"
#include "tss.h"
#include "Console.h"

unsigned int PIT::ticks;
extern void SwitchTask(int tick, registers_t& regs);

#define		I86_PIT_REG_COUNTER0		0x40
#define		I86_PIT_REG_COUNTER1		0x41
#define		I86_PIT_REG_COUNTER2		0x42
#define		I86_PIT_REG_COMMAND			0x43

void PIT::pitHandler()
{
	//TaskSchedule::ProcessSchedule();
}

PIT::PIT(unsigned char chan)
{
	channel = chan;
}

PIT::~PIT()
{
}

void PIT::Enable(unsigned int frequency)
{
	m_frequency = frequency;
	unsigned int divisor = TimerFrequency / m_frequency;

	OutPortByte(0x43, channel == 0 ? 0x36 : 0xB6);
	OutPortByte(0x40 + channel, (unsigned char)(divisor & 0xFF));
	OutPortByte(0x40 + channel, (unsigned char)((divisor >> 8) & 0xFF));
	//AddInterruptHandler(0, pitHandler);
}

void PIT::Disable()
{
	OutPortByte(0x61, InPortByte(0x61) & 0xFC);
}


volatile uint32_t			_pit_ticks = 0;
int g_esp = 0;
uint32_t g_pageDirectory = 0;

void isr_handler(registers_t regs)
{	
	SwitchTask(_pit_ticks, regs);
	//SkyConsole::Print("rrrr : %d\n", _pit_ticks);
}

/*__declspec(naked) void  _cdecl i86_pit_irq()
{
	_asm {
		pushad
	}
	_pit_ticks++;
	SkyConsole::Print("rrrr : %d\n", _pit_ticks);
	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		iretd
	}
}*/


//!	pit timer interrupt handler
__declspec(naked) void i86_pit_irq() 
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
	_pit_ticks++;

	

	_asm
	{
		
		call isr_handler		
	}
		
	__asm
	{
		cmp g_pageDirectory, 0
		jz pass
		mov	eax, [g_pageDirectory]
		mov	cr3, eax		// PDBR is cr3 register in i86
		pass:

		mov eax, g_esp
		mov esp, eax

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

//! starts a counter
void i86_pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode) {

	if (freq == 0)
		return;

	uint16_t divisor = uint16_t(1193181 / (uint16_t)freq);

	//! send operational command
	uint8_t ocw = 0;
	ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
	ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
	i86_pit_send_command(ocw);

	//! set frequency rate
	i86_pit_send_data(divisor & 0xff, 0);
	i86_pit_send_data((divisor >> 8) & 0xff, 0);

	//! reset tick count
	_pit_ticks = 0;
}


//! initialize minidriver
void _cdecl i86_pit_initialize() {

	//! Install our interrupt handler (irq 0 uses interrupt 32)
	setvect(32, i86_pit_irq);
	
}


//! Sets new pit tick count and returns prev. value
uint32_t i86_pit_set_tick_count(uint32_t i) {

	uint32_t ret = _pit_ticks;
	_pit_ticks = i;
	return ret;
}


//! returns current tick count
uint32_t i86_pit_get_tick_count() {

	return _pit_ticks;
}

unsigned int GetTickCount()
{
	return _pit_ticks;
}

void _cdecl msleep(int ms)
{

	int ticks = ms + GetTickCount();
	while (ticks > GetTickCount())
		;
}


//! send command to pit
void i86_pit_send_command(uint8_t cmd) {

	OutPortByte(I86_PIT_REG_COMMAND, cmd);
}


//! send data to a counter
void i86_pit_send_data(uint16_t data, uint8_t counter) {

	uint8_t	port = (counter == I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
		((counter == I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

	OutPortByte(port, (uint8_t)data);
}