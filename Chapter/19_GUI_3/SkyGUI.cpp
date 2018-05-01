#include "SkyGUI.h"
#include "memory.h"
#include "FontData.h"
#include "ProcessManager.h"
#include "sprintf.h"
#include "SkyAPI.h"
#include "mouse.h"
#include "fifo.h"
#include "PhysicalMemoryManager.h"
#include "SkySheetController.h"
#include "SkyRenderer.h"
#include "SkySimpleGUI.h"

extern void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
extern void inthandler2c();
extern void inthandler21();

__declspec(naked) void kSkyMouseHandler()
{

	_asm {
		cli
		pushad
	}

	_asm
	{
		call inthandler2c
	}

	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		sti
		iretd
	}
}
static char keytable0[0x80] = {
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0,   0,   'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',   0,   '\\', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};
static char keytable1[0x80] = {
	0,   0,   '!', '@', '#', '$', '%', '&', '^', '*', '(', ')', '_', '+', 0,   0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,   0,   'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0,   '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

//int key_to = 0, key_shift = 0, key_leds = 0, keycmd_wait = -1;
//char s[30], cmdline[30];


__declspec(naked) void kSkyKeyboardHandler()
{

	_asm {
		cli
		pushad
	}

	_asm
	{
		call inthandler21
	}

	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		sti
		iretd
	}
}

extern char hankaku[4096];

SkyGUI::SkyGUI()
{
}


SkyGUI::~SkyGUI()
{
}

bool SkyGUI::Initialize(void* pVideoRamPtr, int width, int height, int bpp)
{
	m_pVideoRamPtr = (ULONG*)pVideoRamPtr;
	m_width = width;
	m_height = height;
	m_bpp = bpp;

	LoadFontFromMemory();
	
	//초기화면 구성
	MakeInitScreen();
	//입출력 시스템 재구성
	MakeIOSystem();
	
	fifo32_init(&keycmd, 32, keycmd_buf);
	fifo32_init(&fifo, 128, fifobuf);

	//sht_win->Slide(64, 56);
	//sht_win->Updown(2);

	//입력창 태스크(task_a) 생성
	/*sht_win = shtctl->Alloc();
	buf_win = new unsigned char[160 * 52];
	sht_win->SetBuf(buf_win, 144, 52, -1);
	SkyRenderer::MakeWindow8(buf_win, 144, 52, "task_a", 1);
	SkyRenderer::MakeTextBox8(sht_win, 8, 28, 128, 16, COL8_FFFFFF);
	*/

	//콘솔 태스크 쉬트를 구성
	//sht_cons = shtctl->Alloc();
	//buf_cons = new unsigned char[256 * 165];
	//sht_cons->SetBuf(buf_cons, 256, 165, -1);
	//SkyRenderer::MakeWindow8(buf_cons, 256, 165, "console", 0);
	//SkyRenderer::MakeTextBox8(sht_cons, 8, 28, 240, 128, COL8_000000);

	//sht_cons->Slide(32, 4);
	//sht_cons->Updown(1);
	//kEnterCriticalSection();
	//ProcessManager::GetInstance()->CreateProcessFromMemory("Console", console_task, this, PROCESS_KERNEL);
	//kLeaveCriticalSection();
	return true;
}

bool SkyGUI::MakeInitScreen()
{
	m_pVideoRamPtr = (ULONG*)0xFD000000;
	m_width = 1024;
	m_height = 768;
	m_bpp = 8;

	SkyRenderer::InitPalette();

	shtctl = new SkySheetController();
	shtctl->Initialize((unsigned char*)m_pVideoRamPtr, m_width, m_height);;

	//백그라운드 쉬트를 생성
	sht_back = shtctl->Alloc();
	buf_back = new unsigned char[m_width * m_height];
	sht_back->SetBuf(buf_back, m_width, m_height, -1);
	SkyRenderer::InitScreen8((unsigned char *)buf_back, m_width, m_height);

	//마우스 쉬트를 생성
	sht_mouse = shtctl->Alloc();
	sht_mouse->SetBuf(buf_mouse, 16, 16, 99);
	SkyRenderer::InitMouseCursor8((char *)buf_mouse, 99);


	cursor_x = 8;
	cursor_c = COL8_FFFFFF;

	mx = (m_width - 16) / 2;
	my = (m_height - 28 - 16) / 2;

	sht_back->Slide(0, 0);
	sht_mouse->Slide(mx, my);

	sht_back->Updown(0);
	sht_mouse->Updown(3);

	return true;
}

bool SkyGUI::LoadFontFromMemory()
{
	unsigned char* buffer = (unsigned char*)hankaku;
	int bufferIndex = 0;
	int charIndex = 0;

	memset(buffer, 0, 4096);

	int readIndex = 0;

	while (readIndex < 32768) {


		for (int i = 0; i < 512; i++)
		{
			if (fontData[readIndex + i] == '*')
			{

				if (charIndex < 8)
				{
					char a = (char)(1 << (8 - 1 - charIndex));
					buffer[bufferIndex] |= a;
				}



			}

			if ((fontData[readIndex + i] == '*') || (fontData[readIndex + i] == '.'))
			{
				charIndex++;

				if (charIndex >= 8)
				{
					bufferIndex++;
					charIndex = 0;
				}
			}
		}

		readIndex += 512;

	}

	return true;
}

bool SkyGUI::MakeIOSystem()
{
	//키보드, 마우스 핸들러 변경
	kEnterCriticalSection();

	setvect(0x21, kSkyKeyboardHandler);
	setvect(0x2c, kSkyMouseHandler);

	
	//fifo32_init(&fifo, 128, fifobuf);
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	OutPortByte(PIC0_IMR, 0xf8); /* PIT와 PIC1와 키보드를 허가(11111000) */
	OutPortByte(PIC1_IMR, 0xef); /* 마우스를 허가(11101111) */
	

	//fifo32_put(&keycmd, KEYCMD_LED);
	//fifo32_put(&keycmd, key_leds);

	kLeaveCriticalSection();
	return true;
}

bool SkyGUI::Run()
{
	for (;;) 
	{
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* 키보드 컨트롤러에 보낼 데이터가 있으면, 보낸다 */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			OutPortByte(PORT_KEYDAT, keycmd_wait);
		}

		__asm cli
		if (fifo32_status(&fifo) == 0) {
			__asm sti
		}
		else 
		{
			int i = fifo32_get(&fifo);
			__asm sti
			
			if (256 <= i && i <= 511) //키보드 데이터 처리
			{ 				
				ProcessKeyboard(i);
			}			
			else if (512 <= i && i <= 767) //마우스 데이터 처리
			{ 
				ProcessMouse(i);
			}			
		}
	}

	return false;
}

void SkyGUI::ProcessKeyboard(int value)
{
	if (value < 0x80 + 256) { /* 키코드를 문자 코드로 변환 */
		if (key_shift == 0) {
			s[0] = keytable0[value - 256];
		}
		else {
			s[0] = keytable1[value - 256];
		}
	}
	else {
		s[0] = 0;
	}
	
	if (value == 256 + 0xfa) {	/* 키보드가 데이터를 무사하게 받았다 */
		keycmd_wait = -1;
	}
	if (value == 256 + 0xfe) {	/* 키보드가 데이터를 무사하게 받을 수 없었다 */
		wait_KBC_sendready();
		OutPortByte(PORT_KEYDAT, keycmd_wait);
	}
	/* 커서의 재표시 */
	if (cursor_c >= 0)
	{
		SkyRenderer::BoxFill8(sht_win->GetBuf(), sht_win->GetXSize(), cursor_c, cursor_x, 28, cursor_x + 7, 43);
	}
	sht_win->Refresh(cursor_x, 28, cursor_x + 8, 44);
}

void SkyGUI::ProcessMouse(int value)
{
	if (mouse_decode(&mdec, value - 512) != 0) {
		/* 마우스 커서의 이동 */
		mx += mdec.x;
		my += mdec.y;
		if (mx < 0) {
			mx = 0;
		}
		if (my < 0) {
			my = 0;
		}
		if (mx > m_width - 1) {
			mx = m_width - 1;
		}
		if (my > m_height - 1) {
			my = m_height - 1;
		}
		sht_mouse->Slide(mx, my);
		if ((mdec.btn & 0x01) != 0) {
			/* 왼쪽 버튼을 누르고 있으면 sht_win를 움직인다 */
			sht_win->Slide(mx - 80, my - 8);
		}
	}
}