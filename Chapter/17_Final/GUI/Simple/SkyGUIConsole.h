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
	bool Print2(char* pMsg);
	bool PrintCommand(char* pMsg, bool backspace);
	void GetCommandForGUI2(char* commandBuffer, int bufSize, char* driveName);

	VOID GetNewLine();

	bool Clear();
	void FillRect(int x, int y, int w, int h, int col);

	void Update(unsigned long *buf);	
	void PutPixel(ULONG x, ULONG y, ULONG col);
	void PutPixel(ULONG i, ULONG col);
	void PutPixel(ULONG i, unsigned char r, unsigned char g, unsigned char b);

	ULONG GetPixel(ULONG i);
	ULONG GetBPP();
	
protected:
	void PutCursor();

private:
	static ULONG* m_pVideoRamPtr;
	int m_width;
	int m_height;
	int m_bpp;

	SkyRenderer* m_pRenderer;
	int m_yPos;
	int m_xPos;
	int m_lastCommandLength;
};