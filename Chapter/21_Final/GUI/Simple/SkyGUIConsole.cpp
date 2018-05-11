#include "SkyOS.h"
#include "SkyGUIConsole.h"
#include "SkyConsole.h"
#include "SkyConsole.h"
#include "SkyRenderer32.h"
#include "SkyGUI.h"

#define RGB16_565(r,g,b) ((b&31) | ((g&63) << 5 | ((r&31) << 11)))

#define COLOR(r,g,b) ((r<<16) | (g<<8) | b)
#define WHITE COLOR(255,255,255)
#define BLACK COLOR(0,0,0)
#define DARKGRAY COLOR(154,154,154)

extern char skyFontData[4096];
ULONG* SkyGUIConsole::m_pVideoRamPtr = nullptr;

#define PIVOT_X 8
#define PIVOT_Y 16

SkyGUIConsole::SkyGUIConsole()
{
	m_yPos = PIVOT_Y;
	m_xPos = PIVOT_X;
}


SkyGUIConsole::~SkyGUIConsole()
{
}

bool SkyGUIConsole::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
{	
	kEnterCriticalSection();
	InitKeyboard();	
	kLeaveCriticalSection();

	m_pRenderer = new SkyRenderer32();
	SkyGUI::LoadFontFromMemory();
		
	m_pVideoRamPtr = (ULONG*)pVideoRamPtr;
	m_width = width;
	m_height = height;
	m_bpp = bpp;

	unsigned char buf[512];
	sprintf((char*)buf, "XRes : %d", width);
	unsigned char charColor = 0xff;
	m_pRenderer->PutFonts_ASC((char*)m_pVideoRamPtr, m_width, m_xPos, m_yPos, (char)charColor, buf);
	GetNewLine();

	sprintf((char*)buf, "YRes : %d", height);
	m_pRenderer->PutFonts_ASC((char*)m_pVideoRamPtr, m_width, m_xPos, m_yPos, (char)charColor, buf);
	GetNewLine();

	sprintf((char*)buf, "BitsPerPixel : %d", bpp);
	m_pRenderer->PutFonts_ASC((char*)m_pVideoRamPtr, m_width, m_xPos, m_yPos, (char)charColor, buf);
	GetNewLine();

	sprintf((char*)buf, "Ram Virtual Address : %x", (uint32_t)pVideoRamPtr);
	m_pRenderer->PutFonts_ASC((char*)m_pVideoRamPtr, m_width, m_xPos, m_yPos, (char)charColor, buf);
	GetNewLine();

	if(buffertype == MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED)
		m_pRenderer->PutFonts_ASC((char*)m_pVideoRamPtr, m_width, m_xPos, m_yPos, (char)charColor, (unsigned char*)("MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED"));
	else if (buffertype == MULTIBOOT_FRAMEBUFFER_TYPE_RGB)
		m_pRenderer->PutFonts_ASC((char*)m_pVideoRamPtr, m_width, m_xPos, m_yPos, (char)charColor, (unsigned char*)("MULTIBOOT_FRAMEBUFFER_TYPE_RGB"));
	else if (buffertype == MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT)
		m_pRenderer->PutFonts_ASC((char*)m_pVideoRamPtr, m_width, m_xPos, m_yPos, (char)charColor, (unsigned char*)("MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT"));
	GetNewLine();

	return true;
}

bool SkyGUIConsole::Run()
{	
	Thread* pThread = ProcessManager::GetInstance()->GetCurrentTask();
	Process* pProcess = pThread->m_pParent;

	ConsoleManager manager;

	int charWidth = (1024 / 8) - 15;
	char* commandBuffer = new char[charWidth];

	while (1)
	{
		SkyConsole::Print("Command> ");		
		memset(commandBuffer, 0, charWidth);
		//SkyConsole::Print("commandBuffer Address : 0x%x\n", &commandBuffer);	

		SkyConsole::GetCommandForGUI(commandBuffer, charWidth);
		SkyConsole::Print("\n");

		if (manager.RunCommand(commandBuffer) == true)
			break;
	}

	/*while (1)
	{
		kEnterCriticalSection();
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());
		kLeaveCriticalSection();
	}*/

	return false;
}

VOID SkyGUIConsole::GetNewLine()
{
	int x, y;
	ULONG *buf = m_pVideoRamPtr;	
	int bxsize = m_width;
	if ((m_yPos + PIVOT_Y + 16) < m_height) {
		m_yPos += 16; /* 다음 행에 */
	}
	else
	{
		/* 스크롤 */
		for (y = PIVOT_Y; y < (m_height - 16 - PIVOT_Y); y++) {
			for (x = PIVOT_X; x < m_width; x++) {
				buf[x + y * bxsize] = buf[x + (y + 16) * bxsize];
			}
		}
		for (y = m_height - 16 - PIVOT_Y; y < (m_height - PIVOT_Y); y++) {
			for (x = PIVOT_X; x < m_width; x++) {
				buf[x + y * bxsize] = 0x00000000;
			}
		}
	}

	m_xPos = PIVOT_X;
}
bool SkyGUIConsole::Clear()
{
	int i;
	int j;

	for (i = 0; i < (int)m_width; i++)
		for (j = 0; j < (int)m_height; j++)
			m_pVideoRamPtr[j * m_width + i] = BLACK;

	m_xPos = PIVOT_X;
	m_yPos = PIVOT_Y;

	return true;
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
	if (m_pRenderer == nullptr)
		return false;

	//backspace
	if (strlen(pMsg) == 1 && pMsg[0] == 0x08) 
	{
		if (m_xPos > 9 * 8)
		{
			FillRect(m_xPos, m_yPos, 8, 16, 0x00, m_width, m_height, m_bpp);
			m_xPos -= 1 * 8;
			FillRect(m_xPos, m_yPos, 8, 16, 0x00, m_width, m_height, m_bpp);
			FillRect(m_xPos, m_yPos + 12, 8, 4, WHITE, m_width, m_height, m_bpp);
		}
		
		return true;
	}

	unsigned char charColor = 0xff;
	FillRect(m_xPos, m_yPos, 8, 16, 0x00, m_width, m_height, m_bpp);
	
	unsigned char *s = (unsigned char*)pMsg;
	for (; *s != 0x00; s++)
	{
		if (*s == '\n')
		{
			GetNewLine();
			continue;
		}

		m_pRenderer->PutFont((char*)m_pVideoRamPtr, m_width, m_xPos, m_yPos, (char)charColor, skyFontData + *s * 16);
		m_xPos += 8;
	}	
	
	FillRect(m_xPos, m_yPos + 12, 8, 4, WHITE, m_width, m_height, m_bpp);	
	
	return true;
}
