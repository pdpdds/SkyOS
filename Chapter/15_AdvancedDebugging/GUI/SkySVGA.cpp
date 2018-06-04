#include "SkySVGA.h"
#include "svgaguientry.h"
#include "VESA.h"

SkySVGA::SkySVGA()
{
}


SkySVGA::~SkySVGA()
{
}

bool SkySVGA::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
{	
	init_lfb();
	lfb_clear();

	return true;
}

bool SkySVGA::Run()
{	
	StartSampleGui();

	for (;;);
	return false;
}

bool SkySVGA::Print(char* pMsg)
{
	return false;
}

bool SkySVGA::Clear()
{
	return false;
}