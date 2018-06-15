#pragma once
#include "I_HangulEngine.h"

class HangulEngine : public I_HangulEngine
{
public:
	HangulEngine();
	~HangulEngine();

	virtual bool SwitchMode() override;
	virtual bool InputAscii(unsigned char letter) override;
	virtual int GetString(char* buffer) override;
	virtual void Reset() override;
	virtual int DrawText(int iX1, int iY1, int iX2, int iY2, DWORD* pstMemoryAddress, int iX, int iY,
		DWORD stTextColor, DWORD stBackgroundColor, const char* pcString, int iLength) override;

protected:
	bool m_bHangulMode;
};

