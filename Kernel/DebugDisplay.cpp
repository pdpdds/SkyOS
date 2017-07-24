//****************************************************************************
//**
//**    DebugDisplay.cpp
//**    - Provides display capabilities for debugging. Because it is
//**	  specifically for debugging and not final release, we don't
//** 	  care for portability here
//**
//****************************************************************************
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "sprintf.h"
#include "hal.h"

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================

//! video memory
static uint16_t *video_memory = (uint16_t *)0xB8000;

//! current position
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

//! current color
static uint8_t	_color = 0;

//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================

#ifdef _MSC_VER
#pragma warning (disable:4244)
#endif

//! Updates hardware cursor
void DebugUpdateCur(int x, int y) {

	// get location
	uint16_t cursorLocation = y * 80 + x;

	// send location to vga controller to set cursor
	EnterCriticalSection();
	OutPortByte(0x3D4, 14);
	OutPortByte(0x3D5, cursorLocation >> 8); // Send the high byte.
	OutPortByte(0x3D4, 15);
	OutPortByte(0x3D5, cursorLocation);      // Send the low byte.
	LeaveCriticalSection();
}

void scroll() {

	if (cursor_y >= 25) {

		uint16_t attribute = _color << 8;

		//! move current display up one line
		for (int i = 0 * 80; i < 24 * 80; i++)
			video_memory[i] = video_memory[i + 80];

		//! clear the bottom line
		for (int i = 24 * 80; i < 25 * 80; i++)
			video_memory[i] = attribute | ' ';

		cursor_y = 24;
	}
}

//! Displays a character
void DebugPutc(unsigned char c) {

	uint16_t attribute = _color << 8;

	//! backspace character
	if (c == 0x08 && cursor_x)
		cursor_x--;

	//! tab character
	else if (c == 0x09)
		cursor_x = (cursor_x + 8) & ~(8 - 1);

	//! carriage return
	else if (c == '\r')
		cursor_x = 0;

	//! new line
	else if (c == '\n') {
		cursor_x = 0;
		cursor_y++;
	}

	//! printable characters
	else if (c >= ' ') {

		//! display character on screen
		uint16_t* location = video_memory + (cursor_y * 80 + cursor_x);
		*location = c | attribute;
		cursor_x++;
	}

	//! if we are at edge of row, go to new line
	if (cursor_x >= 80) {

		cursor_x = 0;
		cursor_y++;
	}

	//! if we are at the last line, scroll up
	if (cursor_y >= 25)
		scroll();

	//! update hardware cursor
	DebugUpdateCur(cursor_x, cursor_y);
}
//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================

//! Sets new font color
unsigned DebugSetColor(const unsigned c) {

	unsigned t = _color;
	_color = c;
	return t;
}

//! Sets new position
void DebugGotoXY(unsigned x, unsigned y) {

	if (cursor_x <= 80)
		cursor_x = x;

	if (cursor_y <= 25)
		cursor_y = y;

	//! update hardware cursor to new position
	DebugUpdateCur(cursor_x, cursor_y);
}

//! returns position
void DebugGetXY(unsigned* x, unsigned* y) {

	if (x == 0 || y == 0)
		return;

	*x = cursor_x;
	*y = cursor_y;
}

//! returns horzontal width
int DebugGetHorz() {

	return 80;
}

//! returns vertical height
int DebugGetVert() {

	return 24;
}

//! Clear screen
void DebugClrScr(const uint8_t c) {

	//! clear video memory by writing space characters to it
	for (int i = 0; i < 80 * 25; i++)
		video_memory[i] = ' ' | (c << 8);

	//! move position back to start
	DebugGotoXY(0, 0);
}

//! Displays a string
void DebugPuts(char* str) {

	if (!str)
		return;

	//! err... displays a string
	for (unsigned int i = 0; i < strlen(str); i++)
		DebugPutc(str[i]);
}

//! Displays a formatted string
extern "C" {
	int DebugPrintf(const char* str, ...) {

		if (!str)
			return 0;

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
					DebugPutc(c);
					i++;		// go to next character
					break;
				}

						  /*** address of ***/
				case 's': {
					int c = (int&)va_arg(args, char);
					char str[64];
					strcpy(str, (const char*)c);
					DebugPuts(str);
					i++;		// go to next character
					break;
				}

						  /*** integers ***/
				case 'd':
				case 'i': {
					int c = va_arg(args, int);
					char str[32] = { 0 };
					itoa_s(c, 10, str);
					DebugPuts(str);
					i++;		// go to next character
					break;
				}

						  /*** display in hex ***/
						  /*int*/
				case 'X': {
					int c = va_arg(args, int);
					char str[32] = { 0 };
					itoa_s(c, 16, str);
					DebugPuts(str);
					i++;		// go to next character
					break;
				}
						  /*unsigned int*/
				case 'x': {
					unsigned int c = va_arg(args, unsigned int);
					char str[32] = { 0 };
					itoa_s(c, 16, str);
					DebugPuts(str);
					i++;		// go to next character
					break;
				}

				default:
					va_end(args);
					return 1;
				}

				break;

			default:
				DebugPutc(str[i]);
				break;
			}

		}

		va_end(args);
		return i;
	}
}

//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[DebugDisplay.cpp]
//**
//****************************************************************************
//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[DebugDisplay.cpp]
//**
//****************************************************************************
//! something is wrong--bail out

static char* sickpc = " \
                               _______      \n\
                               |.-----.|    \n\
                               ||x . x||    \n\
                               ||_.-._||    \n\
                               `--)-(--`    \n\
                              __[=== o]___  \n\
                             |:::::::::::|\\ \n\
                             `-=========-`()\n\
                                M. O. S.\n\n";

void _cdecl kernel_panic(const char* fmt, ...) {

	EnterCriticalSection();

	va_list		args;
	static char buf[1024];

	va_start(args, fmt);
	va_end(args);

	char* disclamer = "We apologize, MOS has encountered a problem and has been shut down\n\
to prevent damage to your computer. Any unsaved work might be lost.\n\
We are sorry for the inconvenience this might have caused.\n\n\
Please report the following information and restart your computer.\n\
The system has been halted.\n\n";

	DebugClrScr(0x1f);
	DebugGotoXY(0, 0);
	DebugSetColor(0x1f);
	DebugPuts(sickpc);
	DebugPuts(disclamer);

	DebugPrintf("*** STOP: %s", fmt);

	for (;;);
}