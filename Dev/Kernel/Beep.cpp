/*              System Speeaker
*/

#include "Beep.h"
#define SOUND_MAGIC 0xB6


void Sound(uint32_t nFrequence) {
	uint32_t Div;
	uint8_t tmp;

	//Set the PIT to the desired frequency
	Div = 1193180 / nFrequence;
	OutPortByte(0x43, 0xb6);
	OutPortByte(0x42, (uint8_t)(Div));
	OutPortByte(0x42, (uint8_t)(Div >> 8));

	//And play the sound using the PC speaker
	tmp = InPortByte(0x61);
	if (tmp != (tmp | 3)) {
		OutPortByte(0x61, tmp | 3);
	}
}

//make it shutup
void NoSound() {
	uint8_t tmp = InPortByte(0x61) & 0xFC;

	OutPortByte(0x61, tmp);
}

void Beep()
{
	Sound(1000);
	msleep(100);
	NoSound();
}

