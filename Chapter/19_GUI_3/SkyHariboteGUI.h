#pragma once
#include "SkyWindow.h"
#include "windef.h"

class HariboteGUI;

class SkyHariboteGUI : public SkyWindow
{
public:
	SkyHariboteGUI();
	~SkyHariboteGUI();

	virtual bool Initialize(void* pVideoRamPtr, int width, int height, int bpp) override;
	virtual bool Run() override;

	bool LoadFontFromMemory();

private:
	ULONG* m_pVideoRamPtr;
	int m_width;
	int m_height;
	int m_bpp;
	HariboteGUI* m_pHaribote;
};

