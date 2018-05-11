#include "SkyOS.h"
#include "SkyGUIConsole.h"
#include "SkyConsole.h"
#include "SkyConsole.h"
#include "SkyRenderer32.h"
#include "SkyGUI.h"

#define RGB16_565(r,g,b) ((b&31) | ((g&63) << 5 | ((r&31) << 11)))

#define COLOR(r,g,b) ((r<<16) | (g<<8) | b)
#define WHITE COLOR(255,255,255)
#define DARKGRAY COLOR(154,154,154)

extern char skyFontData[4096];
ULONG* SkyGUIConsole::m_pVideoRamPtr = nullptr;

SkyGUIConsole::SkyGUIConsole()
{
}


SkyGUIConsole::~SkyGUIConsole()
{
}

SkyRenderer* renderer;
static int yPos = 80;
static int xPos = 8;

bool SkyGUIConsole::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
{	
	renderer = new SkyRenderer32();
	SkyGUI::LoadFontFromMemory();
		
	m_pVideoRamPtr = (ULONG*)pVideoRamPtr;
	m_width = width;
	m_height = height;
	m_bpp = bpp;

	unsigned char buf[512];
	sprintf((char*)buf, "XRes : %d", width);
	unsigned char charColor = 0xff;
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, 0, (char)charColor, buf);

	sprintf((char*)buf, "YRes : %d", height);
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, 16, (char)charColor, buf);

	sprintf((char*)buf, "BitsPerPixel : %d", bpp);
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, 32, (char)charColor, buf);

	sprintf((char*)buf, "Ram Virtual Address : %x", (uint32_t)pVideoRamPtr);
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, 48, (char)charColor, buf);

	if(buffertype == MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED)
		renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, 8, 64, (char)charColor, (unsigned char*)("MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED"));
	else if (buffertype == MULTIBOOT_FRAMEBUFFER_TYPE_RGB)
		renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, 8, 64, (char)charColor, (unsigned char*)("MULTIBOOT_FRAMEBUFFER_TYPE_RGB"));
	else if (buffertype == MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT)
		renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, 8, 64, (char)charColor, (unsigned char*)("MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT"));

	return true;
}

bool SkyGUIConsole::Run()
{	
	Thread* pThread = ProcessManager::GetInstance()->GetCurrentTask();
	Process* pProcess = pThread->m_pParent;

	while (1)
	{
		kEnterCriticalSection();
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());
		kLeaveCriticalSection();
	}

	return false;
}

void SkyGUIConsole::Clear()
{
	int i;
	int j;

	for (i = 0; i < (int)1024; i++)
		for (j = 0; j < (int)768; j++)
			m_pVideoRamPtr[j * 1024 + i] = WHITE;
}

ULONG SkyGUIConsole::GetBPP() {
	return m_bpp;
}

void SkyGUIConsole::PutPixel(ULONG x, ULONG y, ULONG col) {
	m_pVideoRamPtr[(y * m_width) + x] = col;
}

void SkyGUIConsole::ppo(ULONG *buffer, ULONG i, unsigned char r, unsigned char g, unsigned char b) {
	buffer[i] = (r << 16) | (g << 8) | b;
}

void SkyGUIConsole::pp(ULONG i, unsigned char r, unsigned char g, unsigned char b) {
	m_pVideoRamPtr[i] = (r << 16) | (g << 8) | b;
}

ULONG SkyGUIConsole::GetPixel(ULONG i) {
	return m_pVideoRamPtr[i];
}

void SkyGUIConsole::PutPixel(ULONG i, ULONG col) {
	m_pVideoRamPtr[i] = col;
}

void SkyGUIConsole::FillRect(int x, int y, int w, int h, int col, int actualX, int actualY, int actualByte) {

	if (actualByte == 24)
	{

		char* lfb = (char*)m_pVideoRamPtr;


		for (int k = 0; k < h; k++)
			for (int j = 0; j < w; j++)
			{
				int index = ((j + x) + (k + y) * actualX) * 3;
				lfb[index] = (char)(col >> 0);
				index++;
				lfb[index] = (char)(col >> 8);
				index++;
				lfb[index] = (char)(col >> 16);
			}
	}
	if (actualByte == 32)
	{
		unsigned* lfb = (unsigned*)0xFD000000;
		for (int k = 0; k < h; k++)
			for (int j = 0; j < w; j++)
			{				
				int index = ((j + x) + (k + y) * actualX);
				lfb[index] = col;
			}
	}
}

void SkyGUIConsole::FillRect8(int x, int y, int w, int h, char col, int actualX, int actualY)
{

	char* lfb = (char*)0xFD000000;

	for (int k = 0; k < h; k++)
		for (int j = 0; j < w; j++)
		{
			int index = ((j + x) + (k + y) * actualX);
			lfb[index] = col;
			index++;
		}

}

void SkyGUIConsole::Update(unsigned long *buf) 
{
	unsigned long *p = m_pVideoRamPtr, *p2 = buf;

	for (int c = 0; c<m_width * m_height; c++) 
	{
		*p = *p2;
		p++;
		p2++;
	}

}

bool SkyGUIConsole::Print(char* pMsg)
{
	if (renderer == nullptr)
		return false;

	unsigned char charColor = 0xff;
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, yPos, (char)charColor, (unsigned char*)pMsg);
	if (strlen(pMsg) > 0 && (pMsg[strlen(pMsg) - 1] == '\n'))
	{
		yPos += 16;
		xPos = 8;
	}
	else
	{
		xPos += strlen(pMsg) * 8;
	}

	
	return true;
}
