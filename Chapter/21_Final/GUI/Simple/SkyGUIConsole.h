#pragma once
#include "windef.h"

class SkyGUIConsole
{
public:
	SkyGUIConsole();
	~SkyGUIConsole();

	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);	
	VOID GetNewLine();

	bool Clear();
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
	static ULONG* m_pVideoRamPtr;
	int m_width;
	int m_height;
	int m_bpp;

	SkyRenderer* m_pRenderer;
	int m_yPos;
	int m_xPos;

};