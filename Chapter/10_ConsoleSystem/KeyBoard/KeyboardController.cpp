#include "KeyboardController.h"
#include "Hal.h"
#include "SkyAPI.h"
#include "SkyConsole.h"

extern void SendEOI();

//특수키 상태
bool shift = false;	
bool ctrl = false;
bool alt = false;
bool caps = false;
bool num = false;

unsigned char leds = 0; //LED 마스크
const unsigned int KEYBUFFSIZE = 129;	//키 버퍼 사이즈

Func_Key FKey[10] =		//펑션키에 함수를 대응시켜서 특정 함수를 실행할 수 있게 한다.
{						
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

unsigned char normal[] = {					//키보드 캐릭터 맵
	0x00,0x1B,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
	'q','w','e','r','t','y','u','i','o','p','[',']',0x0D,0x80,
	'a','s','d','f','g','h','j','k','l',';',047,0140,0x80,
	0134,'z','x','c','v','b','n','m',',','.','/',0x80,
	'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,'0',0177
};
//쉬프트키가 눌러진 경우
unsigned char shifted[] = {
	0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
	'Q','W','E','R','T','Y','U','I','O','P','{','}',015,0x80,
	'A','S','D','F','G','H','J','K','L',':',042,'~',0x80,
	'|','Z','X','C','V','B','N','M','<','>','?',0x80,
	'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,'7','8','9',0x80,'4','5','6',0x80,
	'1','2','3','0',177
};

//Caps Lock키가 활성화된 경우
unsigned char capsNormal[] = {
	0x00,0x1B,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
	'Q','W','E','R','T','Y','U','I','O','P','[',']',0x0D,0x80,
	'A','S','D','F','G','H','J','K','L',';',047,0140,0x80,
	'|','Z','X','C','V','B','N','M',',','.','/',0x80,
	'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,'0',0177
};

//쉬프트키 Caps Lock키 둘다 누른경우
unsigned char capsShifted[] = {
	0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
	'q','w','e','r','t','y','u','i','o','p','{','}',015,0x80,
	'a','s','d','f','g','h','j','k','l',':',042,'~',0x80,
	0134,'z','x','c','v','b','n','m','<','>','?',0x80,
	'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,'7','8','9',0x80,'4','5','6',0x80,
	'1','2','3','0',177
};

char buffer[KEYBUFFSIZE];	//키 버퍼
int  buffend = 0;		//버퍼에 저장된 마지막 키값을 가리킨다.
unsigned char scanCode;	//키보드로부터 얻은 스캔코드값


KeyboardController::KeyboardController()
{
}

KeyboardController::~KeyboardController()
{
}

void KeyboardController::UpdateLeds(unsigned char led)	//키보드 LED를 업데이트한다.
{
	if(led == 0)
	{
		leds = 0;
	}
	else
	{
		if (leds == (leds|led))	//LED가 켜져 있다면
		{
			leds = leds^led;	//LED를 끈다
		}
		else
		{
			leds = leds | led;	// LED를 켠다
		}
	}

	//커맨드 바이트를 보내고 결과를 얻을때까지
	//루프를 돈다
	OutPortByte(0x64, 0xED);
	while ((InPortByte(0x64) % 2) == 2)
		;
	//LED 상태를 업데이트 한다.
	OutPortByte(0x60, leds);
	
}

	
//키보드 인터럽트 핸들러
__declspec(naked) void KeyboardHandler()
{
	//레지스터를 저장하고 인터럽트를 끈다.
	_asm
	{
		PUSHAD
		PUSHFD
		CLI
	}

	// 스택상태가 변경되는 것을 막기 위해 함수를 호출한다. 
	_asm call KeyboardController::HandleKeyboardInterrupt

	SendEOI();

	// 레지스터를 복원하고 원래 수행하던 곳으로 돌아간다.
	_asm
	{
		POPFD
		POPAD
		IRETD
	}
}

int KeyboardController::SpecialKey(unsigned char key)
{
	static bool specKeyUp = true;	
	switch (key)
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
		if (specKeyUp == true)
		{
			caps = !caps;
			UpdateLeds(CapsLock);
			specKeyUp = false;
		}
		break;
	case 0x45: //Num down
		if (specKeyUp == true)
		{
			num = !num;
			UpdateLeds(NumLock);
			specKeyUp = false;
		}
		break;
	case 0x46: //Scroll down
		if (specKeyUp == true)
		{
			num = !num;
			UpdateLeds(ScrollLock);
			specKeyUp = false;
		}
		break;
	case 0x3B: //F1 Down
		if (specKeyUp && FKey[0].enabled)
		{
			FKey[0].func();
			specKeyUp = false;
		}
		break;
	case 0x3C: //F2 Down
		if (specKeyUp && FKey[1].enabled)
		{
			FKey[1].func();
			specKeyUp = false;
		}
		break;
	case 0x3D: //F3 Down
		if (specKeyUp && FKey[2].enabled)
		{
			FKey[2].func();
			specKeyUp = false;
		}
		break;
	case 0x3E: //F4 Down
		if (specKeyUp && FKey[3].enabled)
		{
			FKey[3].func();
			specKeyUp = false;
		}
		break;
	case 0x3F: //F5 Down
		if (specKeyUp && FKey[4].enabled)
		{
			FKey[4].func();
			specKeyUp = false;
		}
		break;
	case 0x40: //F6 Down
		if (specKeyUp && FKey[5].enabled)
		{
			FKey[5].func();
			specKeyUp = false;
		}
		break;
	case 0x41: //F7 Down
		if (specKeyUp && FKey[6].enabled)
		{
			FKey[6].func();
			specKeyUp = false;
		}
		break;
	case 0x42: //F8 Down
		if (specKeyUp && FKey[7].enabled)
		{
			FKey[7].func();
			specKeyUp = false;
		}
		break;
	case 0x43: //F9 Down
		if (specKeyUp && FKey[8].enabled)
		{
			FKey[8].func();
			specKeyUp = false;
		}
		break;
	case 0x44: //F10 Down
		if (specKeyUp && FKey[9].enabled)
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

void KeyboardController::FlushBuffers()
{
	unsigned char c = 0;
	while ((c = InPortByte(0x60)) != InPortByte(0x60))
		;
}

void KeyboardController::SetupInterrupts()
{
	FlushBuffers();
	setvect(33, KeyboardHandler);
}

void KeyboardController::SetLEDs(bool scroll, bool number, bool capslk)
{
	//Bit 1 : 스크롤 락 LED를 켠다.
	unsigned char status = scroll ? 1 : 0;

	if (number)	//Bit 2 : Num Lock
		status |= 2;
	if (capslk)//Bit 3:	Caps Lock
		status |= 4;
	//커맨드 버퍼가 비워질때 까지 대기한다.
	while ((InPortByte(0x64) & 2) == 2)
		;
	//커맨드를 보내고 결과가 올때까지 대기
	OutPortByte(0x64, 0xED);
	while ((InPortByte(0x64) % 2) == 2)
		;
	//LED 상태 업데이트를 요청한다.
	OutPortByte(0x60, status);
}

char KeyboardController::GetInput()		//키보드 데이터를 외부에 주기위해 제공되는 메소드
{
	int i = 0;
	while (buffend == 0) //키보드 데이터가 들어올때까지 대기한다.
	{		
		//msleep(10);
	}

	kEnterCriticalSection();		//버퍼를 수정하는 동안 인터럽트를 비활성화시킨다.

	for (; i < buffend; i++)
	{
		buffer[i] = buffer[i + 1];
	}
	buffend--;

	kLeaveCriticalSection();//인터럽트를 활성화한다.

	return buffer[0];
}

void KeyboardController::HandleKeyboardInterrupt()
{
	unsigned char asciiCode;

	scanCode = InPortByte(0x60);	//키 스캔코드를 얻는다.

	if (!(SpecialKey(scanCode) | (scanCode >= 0x80))) //아스키코드라면
	{
		if (shift)		//쉬프트키와 Caps Lock 상태에 따른 적절한 아스키값을 얻어온다.
		{
			if (!caps)
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
			if (!caps)
			{
				asciiCode = normal[scanCode];
			}
			else
			{
				asciiCode = capsNormal[scanCode];
			}
		}

		//키버퍼에 아스키값을 기록한다.
		if (buffend != (KEYBUFFSIZE - 1))
		{
			buffend++;
		}
		buffer[buffend] = asciiCode;
	}
}