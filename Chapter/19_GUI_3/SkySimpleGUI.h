#pragma once
#include "SkyWindow.h"
#include "windef.h"

class SkySimpleGUI : public SkyWindow
{
public:
	SkySimpleGUI();
	~SkySimpleGUI();

	virtual bool Initialize(void* pVideoRamPtr, int width, int height, int bpp) override;
	virtual bool Run() override;

	void Clear();
	ULONG GetBPP();	
	void Update(unsigned long *buf);	
	void PutPixel(ULONG x, ULONG y, ULONG col);
	void PutPixel(ULONG i, ULONG col);
	ULONG GetPixel(ULONG i);
	static void FillRect(int x, int y, int w, int h, int col, int actualX, int actualY, int actualByte);
	static void FillRect8(int x, int y, int w, int h, char col, int actualX, int actualY);

	void ppo(ULONG *buffer, ULONG i, unsigned char r, unsigned char g, unsigned char b);
	void pp(ULONG i, unsigned char r, unsigned char g, unsigned char b);

private:
	ULONG* m_pVideoRamPtr;
	int m_width;
	int m_height;
	int m_bpp;

};