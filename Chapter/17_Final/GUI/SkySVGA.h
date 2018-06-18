#pragma once
#include "stdint.h"
#include "svgagui.h"
#include "lua.h"
#include "lualib.h"
#include "luatinker.h"

class SkySVGA
{
public:
	SkySVGA();
	~SkySVGA();

	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
	bool Clear();

protected:
	int StartGui();

private:
	bool m_exit_program;
	GuiWinThread *m_pWinThread;
	
};

