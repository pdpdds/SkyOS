#include "SkyOS.h"
#include "SkySimpleGUI.h"
#include "SkyConsole.h"

#include "SkyConsole.h"
#include "SkyRenderer32.h"
#include "SkyGUI.h"

#define RGB16_565(r,g,b) ((b&31) | ((g&63) << 5 | ((r&31) << 11)))

#define COLOR(r,g,b) ((r<<16) | (g<<8) | b)
#define WHITE COLOR(255,255,255)
#define DARKGRAY COLOR(154,154,154)

SkySimpleGUI::SkySimpleGUI()
{
}


SkySimpleGUI::~SkySimpleGUI()
{
}

SkyRenderer* renderer;
static int yPos = 80;
static int xPos = 8;

bool SkySimpleGUI::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
{	
	renderer = new SkyRenderer32();
	SkyGUI::LoadFontFromMemory();
	
	/*
	uint16_t *graphics_memory = (uint16_t *)0xA0000;
	graphics_memory[0] = 0xff;
	graphics_memory[1] = 0xff;
	graphics_memory[2] = 0xff;*/
		
	m_pVideoRamPtr = (ULONG*)pVideoRamPtr;
	m_width = width;
	m_height = height;
	m_bpp = bpp;

	unsigned char buf[512];
	sprintf((char*)buf, "XRes : %d", width);
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, 0, 0xFF, buf);

	sprintf((char*)buf, "YRes : %d", height);
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, 16, 0xFF, buf);

	sprintf((char*)buf, "BitsPerPixel : %d", bpp);
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, 32, 0xFF, buf);

	sprintf((char*)buf, "Ram Virtual Address : %x", (uint32_t)pVideoRamPtr);
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, 48, 0xFF, buf);

	if(buffertype == 0)
		renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, 0, 64, 0xFF, (unsigned char*)("MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED"));
	else if (buffertype == 1)
		renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, 0, 64, 0xFF, (unsigned char*)("MULTIBOOT_FRAMEBUFFER_TYPE_RGB"));
	else if (buffertype == 2)
		renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, 0, 64, 0xFF, (unsigned char*)("MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT"));

	return false;
}

extern char hankaku[4096];
bool SkySimpleGUI::Run()
{	
	Thread* pThread = ProcessManager::GetInstance()->GetCurrentTask();
	Process* pProcess = pThread->m_pParent;

	while (1)
	{
		kEnterCriticalSection();
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());
		kLeaveCriticalSection();

		//FillRect(200, 100, 100, 100, 0x00FF0000, 1024, 768, 32);
		//renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, 0, 0, 0xFF, (unsigned char*)"aaaaaaaaaaaaaaaa");
		
		//Clear();
	}

	for (;;);
	return false;
}

void SkySimpleGUI::Clear()
{
	int i;
	int j;

	for (i = 0; i < (int)1024; i++)
		for (j = 0; j < (int)768; j++)
			m_pVideoRamPtr[j * 1024 + i] = WHITE;
}

ULONG SkySimpleGUI::GetBPP() {
	return m_bpp;
}

void SkySimpleGUI::PutPixel(ULONG x, ULONG y, ULONG col) {
	m_pVideoRamPtr[(y * m_width) + x] = col;
}

void SkySimpleGUI::ppo(ULONG *buffer, ULONG i, unsigned char r, unsigned char g, unsigned char b) {
	buffer[i] = (r << 16) | (g << 8) | b;
}

void SkySimpleGUI::pp(ULONG i, unsigned char r, unsigned char g, unsigned char b) {
	m_pVideoRamPtr[i] = (r << 16) | (g << 8) | b;
}

ULONG SkySimpleGUI::GetPixel(ULONG i) {
	return m_pVideoRamPtr[i];
}

void SkySimpleGUI::PutPixel(ULONG i, ULONG col) {
	m_pVideoRamPtr[i] = col;
}

void SkySimpleGUI::FillRect(int x, int y, int w, int h, int col, int actualX, int actualY, int actualByte) {

	if (actualByte == 24)
	{

		char* lfb = (char*)0xFD000000;

		/*for (int i = 0; i < 0x090000; i++)
		{
		lfb[i] = col;
		}*/


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

void SkySimpleGUI::FillRect8(int x, int y, int w, int h, char col, int actualX, int actualY)
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

void SkySimpleGUI::Update(unsigned long *buf) {
	unsigned long c;
	unsigned long *p = m_pVideoRamPtr, *p2 = buf;

	for (c = 0; c<m_width * m_height; c++) {
		*p = *p2;
		p++;
		p2++;
	}

}

bool SkySimpleGUI::Print(char* pMsg)
{
	renderer->PutFonts_ASC((char*)m_pVideoRamPtr, 1024, xPos, yPos, 0xFF, (unsigned char*)pMsg);
	yPos += 16;
	return true;
}
