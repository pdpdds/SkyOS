#pragma once
#include "stdint.h"

class SkySVGA
{
public:
	SkySVGA();
	~SkySVGA();

	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
	bool Clear();
};

