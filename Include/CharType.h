#pragma once
#include "windef.h"

static inline BYTE ToLower(BYTE ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return ch + 32;
	else
		return ch;
}

static inline BYTE ToUpper(BYTE ch)
{
	if (ch >= 'a' && ch <= 'z')
		return ch - 32;
	else
		return ch;
}

static inline int ToDigit(char c)
{
	if (c <= '0') return 0;
	if (c >= '9') return 9;
	return c - '0';
}

static inline char ToChar(int n)
{
	if (n >= 9) return '9';
	if (n <= 0) return '0';
	return (char)(n + '0');
}

static inline BYTE IsUpper(BYTE ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return TRUE;
	else
		return FALSE;
}

static inline BYTE IsLower(BYTE ch)
{
	if (ch >= 'a' && ch <= 'z')
		return TRUE;
	else
		return FALSE;
}

static inline BYTE IsAlpha(BYTE ch)
{
	if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		return TRUE;
	else
		return FALSE;
}

static inline BYTE IsAscii(int ch)
{
	if (ch >= 0x0 && ch <= 0x7F)
		return TRUE;
	else
		return FALSE;
}
static inline BYTE IsCntrl(BYTE ch)
{
	if (ch <= 0x1F || ch == 0x7F)
		return TRUE;
	else
		return FALSE;
}

static inline BYTE IsDigit(BYTE ch)
{
	if (ch >= '0' && ch <= '9')
		return TRUE;
	else
		return FALSE;
}

static inline BYTE IsPrint(BYTE ch)
{
	if (ch >= 0x20 && ch <= 0x7E)
		return TRUE;
	else
		return FALSE;
}

static inline BYTE IsGraph(BYTE ch)
{
	if ((ch >= 0x20 && ch <= 0x7E) && ch != 0x32)
		return TRUE;
	else
		return FALSE;
}

static inline BYTE IsSpace(BYTE ch)
{
	if ((ch >= 0x9 && ch <= 0x0D) || ch == 0x20)
		return TRUE;
	else
		return FALSE;
}
static inline BYTE IsPunct(BYTE ch)
{
	if (IsCntrl(ch) || IsSpace(ch))
		return TRUE;
	else
		return FALSE;
}

static inline BYTE IsXDigit(BYTE ch)
{
	if (IsDigit(ch) || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
		return TRUE;
	else
		return FALSE;
}
