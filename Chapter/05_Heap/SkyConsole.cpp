#include "SkyConsole.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "sprintf.h"

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

		if (c == 0x30) //c can be 0x00 or 0x20 for colour, 0x30 for mono
		{
			m_pVideoMemory = (unsigned short*)0xb0000;
			m_VideoCardType = VGA_MONO_CRT_ADDRESS;	// mono
		}
		else
		{
			m_pVideoMemory = (unsigned short*)0xb8000;
			m_VideoCardType = VGA_COLOR_CRT_ADDRESS;	// color
		}

		m_ScreenHeight = 25;
		m_ScreenWidth = 80;

		m_xPos = 0;
		m_yPos = 0;

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

			*(m_pVideoMemory + t) = ' ' | ((unsigned char)m_Color << 8); // put space under the cursor
			break;

		default:						// -> all other characters
			if (c < ' ') break;				// ignore non printable ascii chars
											//See the article for an explanation of this. Don't forget to add support for new lines

			ushort* VideoMemory = m_pVideoMemory + m_ScreenWidth * m_yPos + m_xPos;
			uchar attribute = (uchar)((backColour << 4) | (textColour & 0xF));

			*VideoMemory = (c | (ushort)(attribute << 8));
			m_xPos++;
			break;
		}

		if (m_xPos >= m_ScreenWidth)
			m_yPos++;

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
					int c = (int&)va_arg(args, char);
					char str[256];
					strcpy(str, (const char*)c);
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

	char	GetChar()
	{
		
		return 0;
	}


	void GetCommand(char* commandBuffer, int bufSize)
	{
		
	}
}