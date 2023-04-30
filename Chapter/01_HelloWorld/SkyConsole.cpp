#include "SkyConsole.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "sprintf.h"


extern "C" int _outp(unsigned short, int);

void OutPortByte(ushort port, uchar value)
{
	_outp(port, value);
}

/*

현 시점에서는 Singleton object 로 만들 수 없기 대문에
Singleton object 느낌이 나도록 namespace 사용

GRUB 사용 시 Global object 를 생성할 수 없음
SkyConsole 도 namespace 를 이용해서 해당 문제를 피함

이유?
GRUB 는 Kernel의 특정 Signature 를 찾아야 하는데,
Program 초반 80K 를 검색해서 찾음

80K 범위 안에 우리 Kernel entry 가 포함되기 위해서는 항상 함수를 선두로 넣음

그런데 Global object 는 항상 선두에 존재
따라서 80K 가 넘을 확률이 높아짐

따라서 Global object 를 생성하면 안됨

이 문제를 해결하려면 GRUB 를 사용하지 않고 직접 Kernel Loader 구현

*/

namespace SkyConsole
{
	static ConsoleColor m_Color;
	static ConsoleColor m_Text;
	static ConsoleColor m_backGroundColor;

	static uint m_xPos;
	static uint m_yPos;

	static ushort* m_pVideoMemory; //Just a pointer to video memory
	static unsigned int m_ScreenHeight;
	static unsigned int m_ScreenWidth;
	static unsigned short m_VideoCardType;

	void Initialize()
	{
		char c = (*(unsigned short*)0x410 & 0x30);//Detects video card type, vga or mono

		// Video 카드가 VGA 인지 흑백인지 확인
		// c can be 0x00 or 0x20 for colour, 0x30 for mono
		if (c == 0x30) 
		{
			// mono 일 경우 Video Memory Address = 0xb0000
			m_pVideoMemory = (unsigned short*)0xb0000;
			m_VideoCardType = VGA_MONO_CRT_ADDRESS;	// mono
		}
		else
		{
			// color 일 경우 Video Memory Address = 0xb8000
			m_pVideoMemory = (unsigned short*)0xb8000;
			m_VideoCardType = VGA_COLOR_CRT_ADDRESS;	// color
		}

		// 화면 사이즈 80*25
		m_ScreenHeight = 25;
		m_ScreenWidth = 80;

		// Cursor위치 초기화
		m_xPos = 0;
		m_yPos = 0;

		// 문자색은 흰색으로 배경은 검은색으로 설정
		m_Text = White;
		m_backGroundColor = Black;
		m_Color = (ConsoleColor)((m_backGroundColor << 4) | m_Text);

		Clear();
	}

	void Clear()
	{

		for (uint i = 0; i < m_ScreenWidth * m_ScreenHeight; i++)				//Remember, 25 rows and 80 columns
		{
			m_pVideoMemory[i] = (ushort)(0x20 | (m_Color << 8));
		}

		MoveCursor(0, 0);
	}

	void Write(const char *szString)
	{
		while ((*szString) != 0)
		{
			WriteChar(*(szString++), m_Text, m_backGroundColor);
		}
	}

	void WriteChar(char c)
	{
		WriteChar(c, m_Text, m_backGroundColor);
	}

	/*
	
	실제로 문자를 출력
	
	+) m_pVideoMemory 를 사용하여 Video Memory 영역에 값을 직접 써넣음

	*/
	void WriteChar(char c, ConsoleColor textColour, ConsoleColor backColour)
	{
		int t;
		switch (c)
		{
		case '\r':                         //-> carriage return
			m_xPos = 0;
			break;

		case '\n':                         // -> newline (with implicit cr)
			m_xPos = 0;
			m_yPos++;
			break;

		case 8:                            // -> backspace
			t = m_xPos + m_yPos * m_ScreenWidth;    // get linear address
			if (t > 0) t--;
			// if not in home position, step back
			if (m_xPos > 0)
			{
				m_xPos--;
			}
			else if (m_yPos > 0)
			{
				m_yPos--;
				m_xPos = m_ScreenWidth - 1;
			}

			// cursor 위치에 있던 문자 제거
			*(m_pVideoMemory + t) = ' ' | ((unsigned char)m_Color << 8); // put space under the cursor
			break;

		default:						// -> all other characters
			if (c < ' ') break;				// ignore non printable ascii chars
											//See the article for an explanation of this. Don't forget to add support for new lines

			// 문자가 출력될 buffer location 을 계산한 뒤 buffer 에 문자를 쓰고 커서의 x 좌표 증가
			ushort* VideoMemory = m_pVideoMemory + m_ScreenWidth * m_yPos + m_xPos;
			uchar attribute = (uchar)((backColour << 4) | (textColour & 0xF));

			*VideoMemory = (c | (ushort)(attribute << 8));
			m_xPos++;
			break;
		}

		// Cursor 의 x 좌표가 화면 너비 이상의 cursor y 좌표값을 증가
		if (m_xPos >= m_ScreenWidth)
			m_yPos++;

		// Cursor 가 다음 화면으로 도달 시 화면 scroll
		if (m_yPos == m_ScreenHeight)			// the cursor moved off of the screen?
		{
			scrollup();					// scroll the screen up
			m_yPos--;						// and move the cursor back
		}
		// and finally, set the cursor

		MoveCursor(m_xPos + 1, m_yPos);
	}

	void WriteString(const char* szString, ConsoleColor textColour, ConsoleColor backColour)
	{
		if (szString == 0)
			return;

		ushort* VideoMemory = 0;

		for (int i = 0; szString[i] != 0; i++)
		{
			VideoMemory = m_pVideoMemory + m_ScreenWidth * m_yPos + i;
			uchar attribute = (uchar)((backColour << 4) | (textColour & 0xF));

			*VideoMemory = (szString[i] | (ushort)(attribute << 8));
		}

		m_yPos++;
		MoveCursor(1, m_yPos);
	}

	/*

	C 나 C++ 을 보면 가변인자를 통한 문자열 출력이 많음
	Print 함수도 이런 처리가 가능

	ex) printf("hello %s!", "SkyOS");

	[% 로 시작하는 인자 종류]
	- s : 문자열 출력
	- c : 문자열 하나 처리, 가변 인자는 char 형
	- d, i : 정수형 처리, 가변 인자는 int 형
	- X : 정수형 처리, 가변 인자는 int 형, hex 로 화면에 출격
	- x : 부호 없는 정수형 처리, 가변 인자는 unsigned int, hex 로 화면에 출력

	[C runtime function]
	- va_start : 가변인자 처리 시작
	- va_arg : 가변인자 처리
	- va_end : 가변 인자 처리 완료
	- itoa_s : 정숫값을 ASCII 값으로 변환

	화면에 문자 출력을 위한 함수의 내부 구현은 전부 우리 몫
	다행히 internet 상에서는 C runtime library 에 대한 구현부가 공개되어 있음
	이렇게 모아놓은 함수들은 CommanLib Project 에 library 화 했으니 참고

	*/

	void Print(const char* str, ...)
	{

		if (!str)
			return;

		va_list		args;
		va_start(args, str);
		size_t i;

		for (i = 0; i < strlen(str); i++) {

			switch (str[i]) {

			case '%':

				switch (str[i + 1]) {

					/*** characters ***/
				case 'c': {
					char c = va_arg(args, char);
					WriteChar(c, m_Text, m_backGroundColor);
					i++;		// go to next character
					break;
				}

						  /*** address of ***/
				case 's': {
					const char * c = (const char *&)va_arg(args, char);
					char str[256];
					strcpy(str, c);
					Write(str);
					i++;		// go to next character
					break;
				}

						  /*** integers ***/
				case 'd':
				case 'i': {
					int c = va_arg(args, int);
					char str[32] = { 0 };
					itoa_s(c, 10, str);
					Write(str);
					i++;		// go to next character
					break;
				}

						  /*** display in hex ***/
						  /*int*/
				case 'X': {
					int c = va_arg(args, int);
					char str[32] = { 0 };
					itoa_s(c, 16, str);
					Write(str);
					i++;		// go to next character
					break;
				}
						  /*unsigned int*/
				case 'x': {
					unsigned int c = va_arg(args, unsigned int);
					char str[32] = { 0 };
					itoa_s(c, 16, str);
					Write(str);
					i++;		// go to next character
					break;
				}

				default:
					va_end(args);
					return;
				}

				break;

			default:
				WriteChar(str[i], m_Text, m_backGroundColor);
				break;
			}

		}

		va_end(args);
	}
	void GetCursorPos(uint& x, uint& y) { x = m_xPos; y = m_yPos; }

	void MoveCursor(unsigned int  X, unsigned int  Y)
	{
		if (X > m_ScreenWidth)
			X = 0;
		unsigned short Offset = (unsigned short)((Y*m_ScreenWidth) + (X - 1));

		OutPortByte(m_VideoCardType, VGA_CRT_CURSOR_H_LOCATION);
		OutPortByte(m_VideoCardType + 1, Offset >> 8);
		OutPortByte(m_VideoCardType, VGA_CRT_CURSOR_L_LOCATION);
		OutPortByte(m_VideoCardType + 1, (Offset << 8) >> 8);

		if (X > 0)
			m_xPos = X - 1;
		else
			m_xPos = 0;

		m_yPos = Y;
	}
	/* Sets the Cursor Type
		0 to 15 is possible value to pass
		Returns - none.
		Example : Normal Cursor - (13,14)
			  Solid Cursor - (0,15)
			  No Cursor - (25,25) - beyond the cursor limit so it is invisible
	*/
	void SetCursorType(unsigned char  Bottom, unsigned char  Top)
	{
		
	}

	void scrollup()		// scroll the screen up one line
	{
		unsigned int t = 0;

		//	disable();	//this memory operation should not be interrupted,
						//can cause errors (more of an annoyance than anything else)
		for (t = 0; t < m_ScreenWidth * (m_ScreenHeight - 1); t++)		// scroll every line up
			*(m_pVideoMemory + t) = *(m_pVideoMemory + t + m_ScreenWidth);
		for (; t < m_ScreenWidth * m_ScreenHeight; t++)				//clear the bottom line
			*(m_pVideoMemory + t) = ' ' | ((unsigned char)m_Color << 8);

		//enable();
	}

	void SetTextColor(ConsoleColor col)
	{						//Sets the colour of the text being displayed
		m_Text = col;
		m_Color = (ConsoleColor)((m_backGroundColor << 4) | m_Text);
	}

	void SetBackColor(ConsoleColor col)
	{						//Sets the colour of the background being displayed
		if (col > LightGray)
		{
			col = Black;
		}
		m_backGroundColor = col;
		m_Color = (ConsoleColor)((m_backGroundColor << 4) | m_Text);
	}

	unsigned char GetTextColor()
	{						//Sets the colour of the text currently set
		return (unsigned char)m_Text;
	}

	unsigned char GetBackColor()
	{						//returns the colour of the background currently set
		return (unsigned char)m_backGroundColor;
	}

	void SetColor(ConsoleColor Text, ConsoleColor Back, bool blink)
	{						//Sets the colour of the text being displayed
		SetTextColor(Text);
		SetBackColor(Back);
		if (blink)
		{
			m_Color = (ConsoleColor)((m_backGroundColor << 4) | m_Text | 128);
		}
	}
}