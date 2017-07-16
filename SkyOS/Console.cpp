#include "Console.h"
#include "SkyHal.h"

Console::Console()
{
	char c = (*(unsigned short*)0x410&0x30);//Detects video card type, vga or mono

	if(c == 0x30) //c can be 0x00 or 0x20 for colour, 0x30 for mono
	{
		m_pVideoMemory = (unsigned short*) 0xb0000;
		m_VideoCardType   = VGA_MONO_CRT_ADDRESS;	// mono
	}
	else
	{
		m_pVideoMemory = (unsigned short*) 0xb8000;
		m_VideoCardType   = VGA_COLOR_CRT_ADDRESS;	// color
	}

	m_ScreenHeight = 25;
	m_ScreenWidth = 80;

	m_xPos = 0;
	m_yPos = 0;

	m_Color = 0x7;
	m_Text = 0x7;
	m_backGroundColor = LightCyan;

	Clear();
}

Console::~Console()
{
}

void Console::Clear()
{
	uchar attribute = (Black << 4) | (White & 0xF);	//Black background, white text

	for(uint i = 0; i < m_ScreenWidth * m_ScreenHeight; i++)				//Remember, 25 rows and 80 columns
	{
		m_pVideoMemory[i] = (ushort)(0x20 | (attribute << 8));
	}

	MoveCursor(0,0);
}

void Console::Write(char *szString)
{
	while((*szString) != 0)
	{
		WriteChar(*(szString++));
	}
}

void Console::WriteChar(char c)
{
	int t;
	switch(c)
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
		if(t > 0) t--;
									   // if not in home position, step back
		if(m_xPos > 0)
		{
			m_xPos--;
		}
		else if(m_yPos > 0)
		{
			m_yPos--;
			m_xPos = m_ScreenWidth - 1;
		}

		*(m_pVideoMemory + t) = ' ' | (m_Color << 8); // put space under the cursor
		break;

	default:						// -> all other characters
		if(c < ' ') break;				// ignore non printable ascii chars
		*(m_pVideoMemory + m_xPos + m_yPos * m_ScreenWidth) = c | (m_Color << 8);
		m_xPos++;							// step cursor one character
		if(m_xPos == m_ScreenWidth)			// to next line if required
		{
			m_xPos = 0;
			m_yPos++;
		}
		break;
	}

	if(m_yPos == m_ScreenHeight)			// the cursor moved off of the screen?
	{
		scrollup();					// scroll the screen up
		m_yPos--;						// and move the cursor back
	}
									// and finally, set the cursor
	MoveCursor(m_xPos+1, m_yPos);
}

void Console::WriteChar(char c, ConsoleColor textColour, ConsoleColor backColour)
{
	//See the article for an explanation of this. Don't forget to add support for new lines

	ushort* VideoMemory = m_pVideoMemory + m_ScreenWidth * m_yPos + m_xPos;
	uchar attribute = (uchar)((backColour << 4) | (textColour & 0xF));

	*VideoMemory = (c | (ushort)(attribute << 8));
	m_xPos++;
}

void Console::WriteString(char* szString, ConsoleColor textColour, ConsoleColor backColour)
{
	if(szString == 0)
		return;

	ushort* VideoMemory = 0;

	for(int i = 0; szString[i] != 0; i++)
	{
		VideoMemory =  m_pVideoMemory + m_ScreenWidth * m_yPos + i;
		uchar attribute = (uchar)((backColour << 4) | (textColour & 0xF));

		*VideoMemory = (szString[i] | (ushort)(attribute << 8));
	}

	m_yPos++;
	MoveCursor(1,m_yPos);
}

void Console::MoveCursor(unsigned int  X,unsigned int  Y)
{
	if(X > m_ScreenWidth)
		X = 0;
	unsigned short Offset = (unsigned short)((Y*m_ScreenWidth) + (X - 1));
	OutPortByte(m_VideoCardType,VGA_CRT_CURSOR_H_LOCATION);
	OutPortByte(m_VideoCardType+1,Offset>>8);
	OutPortByte(m_VideoCardType,VGA_CRT_CURSOR_L_LOCATION);
	OutPortByte(m_VideoCardType+1,(Offset<<8)>>8);
}
/* Sets the Cursor Type
	0 to 15 is possible value to pass
	Returns - none.
	Example : Normal Cursor - (13,14)
		  Solid Cursor - (0,15)
		  No Cursor - (25,25) - beyond the cursor limit so it is invisible
*/
void Console::SetCursorType(unsigned char  Bottom, unsigned char  Top)
{
	OutPortByte(m_VideoCardType,VGA_CRT_CURSOR_START);
	OutPortByte(m_VideoCardType + 1,Top);
	OutPortByte(m_VideoCardType,VGA_CRT_CURSOR_END);
	OutPortByte(m_VideoCardType + 1,Bottom);
}

void Console::scrollup()		// scroll the screen up one line
{
	unsigned int t = 0;

//	disable();	//this memory operation should not be interrupted,
				//can cause errors (more of an annoyance than anything else)
	for(t = 0; t < m_ScreenWidth * (m_ScreenHeight - 1); t++)		// scroll every line up
		*(m_pVideoMemory + t) = *(m_pVideoMemory + t + m_ScreenWidth);
	for(; t < m_ScreenWidth * m_ScreenHeight; t++)				//clear the bottom line
		*(m_pVideoMemory + t) = ' ' | (m_Color << 8);

	//enable();
}

void Console::SetTextColor(ConsoleColor col)
{						//Sets the colour of the text being displayed
	m_Text = (unsigned char)col;
	m_Color = (m_backGroundColor << 4) | m_Text;
}

void Console::SetBackColor(ConsoleColor col)
{						//Sets the colour of the background being displayed
	if(col > 7)
	{
		col = Black;
	}
	m_backGroundColor = (unsigned char)col;
	m_Color = (m_backGroundColor << 4) | m_Text;
}

unsigned char Console::GetTextColor()
{						//Sets the colour of the text currently set
	return (unsigned char)m_Text;
}

unsigned char Console::GetBackColor()
{						//returns the colour of the background currently set
	return (unsigned char)m_backGroundColor;
}

void Console::SetColor(ConsoleColor Text, ConsoleColor Back, bool blink)
{						//Sets the colour of the text being displayed
	SetTextColor(Text);
	SetBackColor(Back);
	if(blink)
	{
		m_Color = (m_backGroundColor << 4) | m_Text | 128;
	}
}
