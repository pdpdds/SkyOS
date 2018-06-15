#pragma once
#include "windef.h"

class I_HangulEngine
{
public:
	virtual bool SwitchMode() = 0; //한영전환
	virtual void Reset() = 0; //새 문자열을 받을 준비를 한다.
	virtual bool InputAscii(unsigned char letter) = 0; //아스키 문자를 집어넣는다.
	virtual int GetString(char* buffer) = 0; //라인 문자열을 얻는다.
	virtual int DrawText(int iX1, int iY1, int iX2, int iY2, DWORD* pstMemoryAddress, int iX, int iY,
		DWORD stTextColor, DWORD stBackgroundColor, const char* pcString, int iLength) = 0;

};