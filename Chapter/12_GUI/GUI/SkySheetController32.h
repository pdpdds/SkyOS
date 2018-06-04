#pragma once
#include "SkySheetController.h"

class SkySheetController32 : public SkySheetController
{
public:
	SkySheetController32();
	virtual ~SkySheetController32();

	virtual bool Initialize(unsigned char *vram, int xsize, int ysize) override;
	virtual void RefreshSub(int vx0, int vy0, int vx1, int vy1, int h0, int h1) override;
	virtual unsigned char* AllocBuffer(int width, int height) override;

protected:
	ULONG* m_vram;
};

