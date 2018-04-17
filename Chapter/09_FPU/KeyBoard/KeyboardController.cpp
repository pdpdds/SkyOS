#include "KeyboardController.h"
#include "Hal.h"
#include "SkyAPI.h"
#include "SkyConsole.h"

Func_Key FKey[10] =		//A table for assigning functions to the function keys
{						// this is done through the keyboard driver
        {false, 0},
        {false, 0},
        {false, 0},
        {false, 0},
        {false, 0},
        {false, 0},
        {false, 0},
        {false, 0},
        {false, 0},
        {false, 0}
};

void UpdateLeds(unsigned char led)	//Updates the keyboard leds
{
	/*if(led == 0)
	{
		leds = 0;
	}
	else
	{
		if (leds == (leds|led))	//If led is already on
		{
			leds = leds^led;	// turn led off
		}
		else
		{
			leds = leds | led;	// else turn led on
		}
	}*/
	//cin.SendData(0xED);
	//cin.SendData(leds);			//update led status
}

int kb_special(unsigned char key);	//Detects non-ascii key presses

	const unsigned int KEYBUFFSIZE = 129;	//Size of our key buffer

	unsigned char normal[] = {					//Keyboard character maps (Look up table)
	  0x00,0x1B,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
	'q','w','e','r','t','y','u','i','o','p','[',']',0x0D,0x80,
	'a','s','d','f','g','h','j','k','l',';',047,0140,0x80,
	0134,'z','x','c','v','b','n','m',',','.','/',0x80,
	'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,'0',0177
	};

	unsigned char shifted[] = {
	  0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
	'Q','W','E','R','T','Y','U','I','O','P','{','}',015,0x80,
	'A','S','D','F','G','H','J','K','L',':',042,'~',0x80,
	'|','Z','X','C','V','B','N','M','<','>','?',0x80,
	'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,'7','8','9',0x80,'4','5','6',0x80,
	'1','2','3','0',177
	};

	unsigned char capsNormal[] = {
	  0x00,0x1B,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
	'Q','W','E','R','T','Y','U','I','O','P','[',']',0x0D,0x80,
	'A','S','D','F','G','H','J','K','L',';',047,0140,0x80,
	'|','Z','X','C','V','B','N','M',',','.','/',0x80,
	'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,'0',0177
	};

	unsigned char capsShifted[] = {
	  0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
	'q','w','e','r','t','y','u','i','o','p','{','}',015,0x80,
	'a','s','d','f','g','h','j','k','l',':',042,'~',0x80,
	0134,'z','x','c','v','b','n','m','<','>','?',0x80,
	'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,'7','8','9',0x80,'4','5','6',0x80,
	'1','2','3','0',177
	};

	bool shift = false;	//Key status
	bool ctrl = false;
	bool alt = false;
	bool caps = false;
	bool num = false;

	unsigned char leds; //The led mask

						//키보드 인터럽트 핸들러
	__declspec(naked) void KeyboardHandler()
	{

		_asm {
			cli
			pushad
		}

		_asm
		{
			call KeyboardController::HandleKeyboardInterrupt
		}

		_asm {
			mov al, 0x20
			out 0x20, al
			popad
			sti
			iretd
		}
	}


	int kb_special(unsigned char key)
	{
		static bool specKeyUp = true;	//Is a key already been or being presses
		switch(key)
		{
			case 0x36: //R-Shift down
			case 0x2A: //L-Shift down
				shift = true;
				break;
			case 0xB6: //R-Shift up
			case 0xAA: //L-Shift up
				shift = false;
				break;
			case 0x1D: //Control down
				ctrl = true;
				break;
			case 0x9D: //Control up
				ctrl = false;
				break;
			case 0x38: //Alt down
				alt = true;
				break;
			case 0xB8: //Alt up
				alt = false;
				break;
			case 0x3A: //Caps down
				if(specKeyUp == true)
				{
					caps = !caps;
					UpdateLeds(CapsLock);
					specKeyUp = false;
				}
				break;
			case 0x45: //Num down
				if(specKeyUp == true)
				{
					num = !num;
					UpdateLeds(NumLock);
					specKeyUp = false;
				}
				break;
			case 0x46: //Scroll down
				if(specKeyUp == true)
				{
					num = !num;
					UpdateLeds(ScrollLock);
					specKeyUp = false;
				}
				break;
			case 0x3B: //F1 Down
				if(specKeyUp && FKey[0].enabled)
				{
					FKey[0].func();
					specKeyUp = false;
				}
				break;
			case 0x3C: //F2 Down
				if(specKeyUp && FKey[1].enabled)
				{
					FKey[1].func();
					specKeyUp = false;
				}
				break;
			case 0x3D: //F3 Down
				if(specKeyUp && FKey[2].enabled)
				{
					FKey[2].func();
					specKeyUp = false;
				}
				break;
			case 0x3E: //F4 Down
				if(specKeyUp && FKey[3].enabled)
				{
					FKey[3].func();
					specKeyUp = false;
				}
				break;
			case 0x3F: //F5 Down
				if(specKeyUp && FKey[4].enabled)
				{
					FKey[4].func();
					specKeyUp = false;
				}
				break;
			case 0x40: //F6 Down
				if(specKeyUp && FKey[5].enabled)
				{
					FKey[5].func();
					specKeyUp = false;
				}
				break;
			case 0x41: //F7 Down
				if(specKeyUp && FKey[6].enabled)
				{
					FKey[6].func();
					specKeyUp = false;
				}
				break;
			case 0x42: //F8 Down
				if(specKeyUp && FKey[7].enabled)
				{
					FKey[7].func();
					specKeyUp = false;
				}
				break;
			case 0x43: //F9 Down
				if(specKeyUp && FKey[8].enabled)
				{
					FKey[8].func();
					specKeyUp = false;
				}
				break;
			case 0x44: //F10 Down
				if(specKeyUp && FKey[9].enabled)
				{
					FKey[9].func();
					specKeyUp = false;
				}
				break;
			case 0xBA: //Caps Up
			case 0xBB: //F1 Up
			case 0xBC: //F2 Up
			case 0xBD: //F3 Up
			case 0xBE: //F4 Up
			case 0xBF: //F5 Up
			case 0xC0: //F6 Up
			case 0xC1: //F7 Up
			case 0xC2: //F8 Up
			case 0xC3: //F9 Up
			case 0xC4: //F10 Up
			case 0xC5: //Num Up
			case 0xC6: //Scroll Up
				specKeyUp = true;
				break;
			case 0xE0:
				break;
			default:
				return(0);
		}
		return (1);
	}


char buffer[KEYBUFFSIZE];	//Our key buffer. Global so that our keyboard driver can access it
int  buffend = 0;		//Last key in the buffer for the driver
unsigned char scanCode;	//Last scan code recieved from the keyboard

KeyboardController::KeyboardController()
{
}

KeyboardController::~KeyboardController()
{
}

void KeyboardController::FlushBuffers()
{
	unsigned char c = 0;

	//The keyboard buffer is empty when two consecutive reads provide the same value
	//Note: this is highly destructive, and will wipe the keyboard buffer. Use with caution
	while((c = InPortByte(0x60)) != InPortByte(0x60))
		;
}

void KeyboardController::ResetComputer()
{
	//Loop until we've got a clean command buffer
	while((InPortByte(0x64) & 2) == 2)
		;
	OutPortByte(0x64, 0xFE);
}



void KeyboardController::SetupInterrupts()
{
	//Wipe the buffer clean, because Bochs and some real hardware misbehaves and
	//stores the key pressed to load our OS from the GrUB menu, causing a freak IRQ
	FlushBuffers();
	setvect(33, KeyboardHandler);
	//You need to have a method to hook interrupts; otherwise, this code won't compile
	//AddInterruptHandler(1, kbInterrupt);
	//IDT::setEntry()
}

void KeyboardController::SetLEDs(bool scroll, bool number, bool capslk)
{
	//Bit 1 set: scroll lock on
	unsigned char status = scroll ? 1 : 0;

	if(number)	//Bit 2: num lock
		status |= 2;
	if(capslk)//Bit 3: caps lock
		status |= 4;
	//Make sure the command buffer is clean
	while((InPortByte(0x64) & 2) == 2)
		;
	//Send the command byte
	OutPortByte(0x64, 0xED);
	//Loop again
	while((InPortByte(0x64) % 2) == 2)
		;
	//Send the status byte
	OutPortByte(0x60, status);
}

char KeyboardController::GetInput()		// Waits for a key to enter the buffer and returns it
{
	int i = 0;
	while (buffend == 0)
	{		
		//kEnterCriticalSection();		//Disable interrupts while we modify the buffer
//		kLeaveCriticalSection();		//Disable interrupts while we modify the buffer
		//msleep(10);
	}

	kEnterCriticalSection();		//Disable interrupts while we modify the buffer

	for(; i < buffend; i++)
	{
		buffer[i] = buffer[i + 1];
	}
	buffend--;

	kLeaveCriticalSection();

	return buffer[0];
}

void KeyboardController::HandleKeyboardInterrupt()
{	
	unsigned char asciiCode;
	kEnterCriticalSection();			//Don't let interrupts bother us while handling one.
	
	scanCode = InPortByte(0x60);	//retrieve scan code

		if(!(kb_special(scanCode) | (scanCode >= 0x80)))
		{
			if(shift)		//Determine key that has been pressed
			{
				if(!caps)
				{
					asciiCode = shifted[scanCode];
				}
				else
				{
					asciiCode = capsShifted[scanCode];
				}
			}
			else
			{
				if(!caps)
				{
					asciiCode = normal[scanCode];
				}
				else
				{
					asciiCode = capsNormal[scanCode];
				}
			}

			if(buffend != (KEYBUFFSIZE - 1))
			{
				buffend++;
			}
			buffer[buffend] = asciiCode;
		}

	kLeaveCriticalSection();			//Enable interrupts
}

