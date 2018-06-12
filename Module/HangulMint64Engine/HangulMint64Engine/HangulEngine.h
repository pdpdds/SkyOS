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

protected:
	bool m_bHangulMode;
};

