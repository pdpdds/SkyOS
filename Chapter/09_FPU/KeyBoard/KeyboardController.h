#pragma once

#include "windef.h"

//Led Numbers
#define ScrollLock (unsigned char)0x01
#define NumLock (unsigned char)0x02
#define CapsLock (unsigned char)0x04

//----typeamatic rate and delay values:----
/*													Value Rate    Value Rate
     This chart is a partial guide for the repeat    0  = 30.0    0Ah = 10.0
     rate (bits 0-4).  You can interpolate for       1  = 26.7    0Dh =  9.2
     values not shown, but let's face it, you're     2  = 24.0    10h =  7.5
     only interested in the fastest rates.           4  = 20.0    14h =  5.0
                                                     8  = 15.0    1Fh =  2.0
  Delay values:
  00=250ms; 01=500ms; 10=750ms; 11=1000ms
*/
#define KDefault 0x02	//Default Delay-Rate

#define K25Delay 0x00	//Delay values (25 fastest -> 01 Slowest)
#define K50Delay 0x20
#define K75Delay 0x40
#define K01Delay 0x60

#define K0Rate 0x0		//Rate values (0 fastest -> L4 Slowest)
#define K1Rate 0x1
#define K2Rate 0x2
#define K4Rate 0x4
#define K8Rate 0x8
#define KL0Rate 0x0A
#define KL1Rate 0x0D
#define KL2Rate 0x10
#define KL3Rate 0x14
#define KL4Rate 0x1F

typedef struct Func_Key_tag	//Function key structure
{
	bool enabled;
	void (* func)();
}Func_Key;

class KeyboardController
{
private:
	KeyboardController();
	~KeyboardController();
public:
	static void HandleKeyboardInterrupt();
	static char GetInput();		// Waits for a key to enter the buffer and returns it

	static void FlushBuffers();
	static void ResetComputer();
	static void SetupInterrupts();
	static void SetLEDs(bool scroll, bool num, bool caps);
};
