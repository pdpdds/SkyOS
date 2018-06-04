#ifndef SKYCONSOLE_H
#define SKYCONSOLE_H
#include "windef.h"

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

namespace SkyConsole
{
	void Initialize();
	void Clear();
	void WriteChar(char c, ConsoleColor textColour, ConsoleColor backColour);
	void WriteString(const char* szString, ConsoleColor textColour = White, ConsoleColor backColour = Black);
	void Write(const char *szString);
	void WriteChar(char c);

	void Print(const char* str, ...);

	void MoveCursor(unsigned int  X, unsigned int  Y);
	void GetCursorPos(uint& x, uint& y);
	void SetCursorType(unsigned char  Bottom, unsigned char Top);
	void scrollup();

	void SetColor(ConsoleColor Text, ConsoleColor Back, bool blink);
	unsigned char GetBackColor();
	unsigned char GetTextColor();
	void SetBackColor(ConsoleColor col);
	void SetTextColor(ConsoleColor col);

	void GetCommand(char* commandBuffer, int bufSize);
	//KEYCODE	GetChar(); 
	char	GetChar();
}
#endif
