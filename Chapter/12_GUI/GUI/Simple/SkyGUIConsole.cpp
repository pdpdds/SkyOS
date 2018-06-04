#include "SkyOS.h"
#include "SkyGUIConsole.h"
#include "SkyConsole.h"
#include "SkyConsole.h"
#include "SkyRenderer32.h"
#include "SkyGUI.h"
#include "KeyboardController.h"

#define COLOR(r,g,b) ((r<<16) | (g<<8) | b)
#define WHITE COLOR(255,255,255)
#define BLACK COLOR(0,0,0)
#define DARKGRAY COLOR(154,154,154)

extern char skyFontData[4096];
ULONG* SkyGUIConsole::m_pVideoRamPtr = nullptr;

#define PIVOT_X 8
#define PIVOT_Y 16
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16
#define CHAR_COLOR 0xff

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
	KeyboardController::SetupInterrupts();
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

	ProcessManager::GetInstance()->CreateProcessFromMemory("GUIWatchDog", GUIWatchDogProc, NULL, PROCESS_KERNEL);

	ConsoleManager manager;

	int bufferLen = (m_width / CHAR_WIDTH) - 15;
	char* commandBuffer = new char[bufferLen];

	while (1)
	{
		SkyConsole::Print("Command> ");		
		memset(commandBuffer, 0, bufferLen);		

		SkyConsole::GetCommandForGUI(commandBuffer, bufferLen);
		SkyConsole::Print("\n");

		if (manager.RunCommand(commandBuffer) == true)
			break;
	}

	return false;
}

VOID SkyGUIConsole::GetNewLine()
{
	int x, y;
	ULONG *buf = m_pVideoRamPtr;	
	int bxsize = m_width;
	if ((m_yPos + PIVOT_Y + CHAR_HEIGHT) < m_height) 
	{
		m_yPos += CHAR_HEIGHT; //커서를 다음행으로 옮긴다.
	}
	else
	{
		//화면을 스크롤한다.
		for (y = PIVOT_Y; y < (m_height - CHAR_HEIGHT - PIVOT_Y); y++)
		{
			for (x = PIVOT_X; x < m_width; x++) 
			{
				buf[x + y * bxsize] = buf[x + (y + CHAR_HEIGHT) * bxsize];
			}
		}
		for (y = m_height - CHAR_HEIGHT - PIVOT_Y; y < (m_height - PIVOT_Y); y++) 
		{
			for (x = PIVOT_X; x < m_width; x++) 
			{
				buf[x + y * bxsize] = 0x00000000;
			}
		}
	}

	m_xPos = PIVOT_X;
}
bool SkyGUIConsole::Clear()
{
	m_xPos = PIVOT_X;
	m_yPos = PIVOT_Y;

	memset(m_pVideoRamPtr, BLACK, (m_width * m_height) * sizeof(ULONG));

	return true;
}

ULONG SkyGUIConsole::GetBPP()
{
	return m_bpp;
}

void SkyGUIConsole::PutPixel(ULONG x, ULONG y, ULONG col) 
{
	m_pVideoRamPtr[(y * m_width) + x] = col;
}

ULONG SkyGUIConsole::GetPixel(ULONG i) {
	return m_pVideoRamPtr[i];
}

void SkyGUIConsole::PutPixel(ULONG i, ULONG col) {
	m_pVideoRamPtr[i] = col;
}

void SkyGUIConsole::FillRect(int x, int y, int w, int h, int col) 
{
	unsigned* lfb = (unsigned*)m_pVideoRamPtr;
	for (int k = 0; k < h; k++)
		for (int j = 0; j < w; j++)
		{
			int index = ((j + x) + (k + y) * m_width);
			lfb[index] = col;
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

	//백스페이스
	if (strlen(pMsg) == 1 && pMsg[0] == 0x08) 
	{
		if (m_xPos > 9 * 8)
		{
			FillRect(m_xPos, m_yPos, CHAR_WIDTH, CHAR_HEIGHT, 0x00);
			m_xPos -= 1 * 8;
			FillRect(m_xPos, m_yPos, CHAR_WIDTH, CHAR_HEIGHT, 0x00);

			PutCursor();
		}
		
		return true;
	}

	FillRect(m_xPos, m_yPos, CHAR_WIDTH, CHAR_HEIGHT, 0x00);
	
	unsigned char *s = (unsigned char*)pMsg;
	for (; *s != 0x00; s++)
	{
		if (*s == '\n')
		{
			GetNewLine();
			continue;
		}

		m_pRenderer->PutFont((char*)m_pVideoRamPtr, m_width, m_xPos, m_yPos, CHAR_COLOR, skyFontData + *s * 16);
		m_xPos += CHAR_WIDTH;
	}	
	
	PutCursor();
	
	return true;
}

void SkyGUIConsole::PutCursor()
{
	FillRect(m_xPos, m_yPos + (CHAR_HEIGHT - 4), CHAR_WIDTH, 4, WHITE);
}

void SkyGUIConsole::PutPixel(ULONG i, unsigned char r, unsigned char g, unsigned char b) {
	m_pVideoRamPtr[i] = (r << 16) | (g << 8) | b;
}