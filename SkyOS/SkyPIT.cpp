#include "SkyPIT.h"
#include "SkyHal.h"
#include "DebugDisplay.h"

void  interrupt _cdecl HandleTimerInterrupt() 
{
	//static int count = 0;
	
	_asm add esp, 12
	_asm pushad

	//count++;
	//if ((count % 10) == 0)
	//	DebugPrintf("\nTimer Count : %d", count);
	
	//! tell hal we are done
	interruptdone(0);

	_asm popad
	_asm iretd	
}


//! send data to a counter
void i86_pit_send_data(uint16_t data, uint8_t counter) {

	uint8_t	port = (counter == I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
		((counter == I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

	OutPortByte(port, (uint8_t)data);
}

//! send command to pit
void i86_pit_send_command(uint8_t cmd) {

	OutPortByte(I86_PIT_REG_COMMAND, cmd);
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
	///_pit_ticks = 0;
}