#ifndef Console_h
#define Console_h
#include "StdIntTypes.h"

//This gives us a little type-safety, so that we don't write a bad value somewhere along the lines
enum ConsoleColor
{
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	LightBrown = 14,
	White = 15
};

#define VGA_COLOR_CRT_ADDRESS 0x3D4
#define VGA_MONO_CRT_ADDRESS 0x3B4

#define VGA_CRT_CURSOR_START 0x0A
#define VGA_CRT_CURSOR_END 0x0B
#define VGA_CRT_H_START_ADDRESS 0x0C
#define VGA_CRT_H_END_ADDRESS 0x0D
#define VGA_CRT_CURSOR_H_LOCATION 0x0E
#define VGA_CRT_CURSOR_L_LOCATION 0x0F

//This is just a basic console class. You'll want to add support for strings, newlines, formatted strings and integers	
class Console
{
private:
	uint m_xPos;
	uint m_yPos;
public:
	Console();
	~Console();
	void Clear();
	void WriteChar(char c, ConsoleColor textColour, ConsoleColor backColour);
	void WriteString(char* szString, ConsoleColor textColour, ConsoleColor backColour);
	void Write(char *szString);
	void WriteChar(char c);

	void MoveCursor(unsigned int  X,unsigned int  Y);
	void SetCursorType(unsigned char  Bottom, unsigned char Top);
	void scrollup();

	void SetColor(ConsoleColor Text, ConsoleColor Back, bool blink);
	unsigned char GetBackColor();
	unsigned char GetTextColor();
	void SetBackColor(ConsoleColor col);
	void SetTextColor(ConsoleColor col);

private:
	ushort* m_pVideoMemory; //Just a pointer to video memory
	unsigned int m_ScreenHeight;
	unsigned int m_ScreenWidth;
	unsigned short m_VideoCardType;

	unsigned char m_Color;
	unsigned char m_Text;
	unsigned char m_backGroundColor;
};
#endif
