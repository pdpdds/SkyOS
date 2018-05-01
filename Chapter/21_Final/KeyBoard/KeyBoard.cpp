#include <string.h>
#include <ctype.h>
#include <hal.h>
#include "KeyBoard.h"
#include "SkyConsole.h"

namespace KeyBoard
{
	static KeyBoardState _keyBoardState;
	
	
	//키보드 인터럽트 핸들러
	__declspec(naked) void i86_kybrd_irq() 
	{

		_asm {
			cli
			pushad
		}

		_asm
		{
			call ProcessKeyBoardInterrupt
		}

		_asm {
			mov al, 0x20
			out 0x20, al
			popad
			sti
			iretd
		}
	}

	bool	GetScrollLock()
	{

		return _keyBoardState._scrolllock;
	}

	bool	GetNumLock()
	{

		return _keyBoardState._numlock;
	}

	bool	GetCapsLock()
	{

		return _keyBoardState._capslock;
	}

	bool	GetCtrl() 
	{

		return _keyBoardState._ctrl;
	}

	bool	GetAlt() 
	{

		return _keyBoardState._alt;
	}

	bool	GetShift()
	{

		return _keyBoardState._shift;
	}

	uint8_t GetLastScanCode() 
	{

		return _keyBoardState._scancode;
	}

	KEYCODE GetLastKeyCode() 
	{
		return (_keyBoardState._scancode != INVALID_SCANCODE) ? ((KEYCODE)KeyboardScanCode[_keyBoardState._scancode]) : (KEY_UNKNOWN);
	}

	void DiscardLastKeyCode()
	{

		_keyBoardState._scancode = INVALID_SCANCODE;
	}

//키보드 제어 관련 메소드
	void SetLeds(bool num, bool caps, bool scroll) {

		uint8_t data = 0;

		data = (scroll) ? (data | 1) : (data & 1);
		data = (num) ? 3 : 0;
		data = (caps) ? 0b101 : 0;

		SendEncodeCommand(KYBRD_ENC_CMD_SET_LED);
		SendEncodeCommand(data);
	}

	//키보드 컨트롤러에게 현재 키입력 데이터를 읽을 수 있는지를 요청한다
	uint8_t ReadStatus()
	{

		return InPortByte(KYBRD_CTRL_STATS_REG);
	}

	//키보드 컨트롤러에게 커맨드 전송
	void SendCommand(uint8_t cmd)
	{

		//키보드 컨트롤러의 입력버퍼가 클리어될때까지 대기한다.
		while (1)
			if ((ReadStatus() & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
				break;

		OutPortByte(KYBRD_CTRL_CMD_REG, cmd);
	}

	//키보드 컨트롤러로부터 인코딩된 키버퍼 데이터를 읽어들인다.
	uint8_t ReadEncodeBuffer() 
	{

		return InPortByte(KYBRD_ENC_INPUT_BUF);
	}

	//키보드에 명령을 전송한다.
	void SendEncodeCommand(uint8_t cmd) 
	{

		//키보드로부터 읽어들일 키 데이터가 있으면 클리어될때까지 대기한다.
		while (1)
			if ((ReadStatus() & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
				break;

		OutPortByte(KYBRD_ENC_CMD_REG, cmd);
	}


	//키코드를 아스키코드로 변환해서 리턴한다.
	char ConvertKeyToAscii(KEYCODE code) 
	{

		uint8_t key = (uint8_t)code;

		//아스키코드인지 확인
		if (isascii(key)) {

			//쉬프트키와 Caps 키 여부를 체크해서 알파벳을 대문자로 변경한다.
			if (_keyBoardState._shift || _keyBoardState._capslock)
				if (key >= 'a' && key <= 'z')
					key -= 32;

			if (_keyBoardState._shift)
				if (key >= '0' && key <= '9')
					switch (key) {

					case '0':
						key = KEY_RIGHTPARENTHESIS;
						break;
					case '1':
						key = KEY_EXCLAMATION;
						break;
					case '2':
						key = KEY_AT;
						break;
					case '3':
						key = KEY_EXCLAMATION;
						break;
					case '4':
						key = KEY_HASH;
						break;
					case '5':
						key = KEY_PERCENT;
						break;
					case '6':
						key = KEY_CARRET;
						break;
					case '7':
						key = KEY_AMPERSAND;
						break;
					case '8':
						key = KEY_ASTERISK;
						break;
					case '9':
						key = KEY_LEFTPARENTHESIS;
						break;
					}
				else {

					switch (key) {
					case KEY_COMMA:
						key = KEY_LESS;
						break;

					case KEY_DOT:
						key = KEY_GREATER;
						break;

					case KEY_SLASH:
						key = KEY_QUESTION;
						break;

					case KEY_SEMICOLON:
						key = KEY_COLON;
						break;

					case KEY_QUOTE:
						key = KEY_QUOTEDOUBLE;
						break;

					case KEY_LEFTBRACKET:
						key = KEY_LEFTCURL;
						break;

					case KEY_RIGHTBRACKET:
						key = KEY_RIGHTCURL;
						break;

					case KEY_GRAVE:
						key = KEY_TILDE;
						break;

					case KEY_MINUS:
						key = KEY_UNDERSCORE;
						break;

					case KEY_PLUS:
						key = KEY_EQUAL;
						break;

					case KEY_BACKSLASH:
						key = KEY_BAR;
						break;
					}
				}

				return key;
		}

		return 0;
	}

	//키보드를 비활성화시킨다.
	void Disable() 
	{

		SendCommand(KYBRD_CTRL_CMD_DISABLE);
		_keyBoardState._disable = true;
	}

	//키보드를 활성화시킨다.
	void Enable() 
	{

		SendCommand(KYBRD_CTRL_CMD_ENABLE);
		_keyBoardState._disable = false;
	}

	//키보드의 활성화여부를 알려준다.
	bool IsDisabled() 
	{

		return _keyBoardState._disable;
	}

	//키보드 컨트롤러를 리셋
	void ResetSystem() 
	{
		SendCommand(KYBRD_CTRL_CMD_WRITE_OUT_PORT);
		SendEncodeCommand(0xfe);
	}

	//키보드 컨트롤러 테스트
	bool SelfTest() {

		SendCommand(KYBRD_CTRL_CMD_SELF_TEST);

		while (1)
			if (ReadStatus() & KYBRD_CTRL_STATS_MASK_OUT_BUF)
				break;

		return (ReadEncodeBuffer() == 0x55) ? true : false;
	}

	//키보드를 설치한다.
	void Install(int irq) {

		//키보드 인터럽트 핸들러 설치
		setvect(irq, i86_kybrd_irq);

		_keyBoardState._alt = false;
		_keyBoardState._ctrl = false;
		_keyBoardState._shift = false;

		_keyBoardState._scancode = 0;
		_keyBoardState._error = 0;

		_keyBoardState._batFailed = false;
		_keyBoardState._diagnosticsFailed = false;
		_keyBoardState._disable = false;
		_keyBoardState._resendCmd = false;

		SetLeds(false, false, false);
	}

//키보드 인터럽트 핸들러
	void ProcessKeyBoardInterrupt()
	{
		static bool _extended = false;
		uint8_t code = 0;

		//버퍼에 키입력 데이터가 들어왔는지 확인한다.
		if (ReadStatus() & KYBRD_CTRL_STATS_MASK_OUT_BUF)
		{

			//스캔코드를 읽어들인다.
			code = ReadEncodeBuffer();

			//확장 스캔 코드는 다루지 않는다.
			if (code == 0xE0 || code == 0xE1)
				_extended = true;
			else 
			{
				_extended = false;

				//이 값이 참이면 키값을 무효화시킨다.
				//입력된 스캔코드값을 플래그에 저장한다.
				if (code & 0x80) //0b10000000
				{	

					code -= 0x80;
					
					int key = KeyboardScanCode[code];
					switch (key) {

					case KEY_LCTRL:
					case KEY_RCTRL:
						_keyBoardState._ctrl = false;
						break;

					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_keyBoardState._shift = false;
						break;

					case KEY_LALT:
					case KEY_RALT:
						_keyBoardState._alt = false;
						break;
					}
				}
				else 
				{

					

					//특수키가 눌려졌는지 확인한다.
					int key = KeyboardScanCode[code];

					switch (key) {

					case KEY_LCTRL:
					case KEY_RCTRL:
						_keyBoardState._ctrl = true;
						break;

					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_keyBoardState._shift = true;
						break;

					case KEY_LALT:
					case KEY_RALT:
						_keyBoardState._alt = true;
						break;

					case KEY_CAPSLOCK:
						_keyBoardState._capslock = (_keyBoardState._capslock) ? false : true;
						SetLeds(_keyBoardState._numlock, _keyBoardState._capslock, _keyBoardState._scrolllock);
						break;

					case KEY_KP_NUMLOCK:
						_keyBoardState._numlock = (_keyBoardState._numlock) ? false : true;
						SetLeds(_keyBoardState._numlock, _keyBoardState._capslock, _keyBoardState._scrolllock);
						break;

					case KEY_SCROLLLOCK:
						_keyBoardState._scrolllock = (_keyBoardState._scrolllock) ? false : true;
						SetLeds(_keyBoardState._numlock, _keyBoardState._capslock, _keyBoardState._scrolllock);
						break;
					default:
						//스캔 코드 저장
						_keyBoardState._scancode = code;

					}
				}
			}

			//에러가 발생했으면 플래그에 세팅
			switch (code)
			{

			case KYBRD_ERR_BAT_FAILED:
				_keyBoardState._batFailed = true;
				break;

			case KYBRD_ERR_DIAG_FAILED:
				_keyBoardState._diagnosticsFailed = true;
				break;

			case KYBRD_ERR_RESEND_CMD:
				_keyBoardState._resendCmd = true;
				break;
			}
		}
	}
}