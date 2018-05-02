#include "SkyGUI.h"
#include "SkyOS.h"
#include "FontData.h"
#include "ProcessManager.h"
#include "SkyMouse.h"
#include "fifo.h"
#include "PhysicalMemoryManager.h"
#include "SkySheetController8.h"
#include "SkySheetController32.h"
#include "SkyRenderer.h"
#include "SkySimpleGUI.h"
#include "SkyRenderer8.h"
#include "SkyRenderer32.h"
#include "Scheduler.h"
#include "KernelProcedure.h"
#include "ConsoleIOListener.h"
#include "Process.h"
#include "Thread.h"

#define DMA_PICU1       0x0020
#define DMA_PICU2       0x00A0
__declspec(naked) void SendEOI()
{
	_asm
	{
		PUSH EBP
		MOV  EBP, ESP
		PUSH EAX

		; [EBP] < -EBP
		; [EBP + 4] < -RET Addr
		; [EBP + 8] < -IRQ 번호

		MOV AL, 20H; EOI 신호를 보낸다.
		OUT DMA_PICU1, AL

		CMP BYTE PTR[EBP + 8], 7
		JBE END_OF_EOI
		OUT DMA_PICU2, AL; Send to 2 also

		END_OF_EOI :
		POP EAX
		POP EBP
		RET
	}
}

extern void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
extern void inthandler2c();
extern void inthandler21();

__declspec(naked) void kSkyMouseHandler()
{

	_asm {
		PUSHAD
		PUSHFD
		CLI
	}

	_asm
	{
		call inthandler2c
	}

	SendEOI();

	_asm
	{
		POPFD
		POPAD
		IRETD
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

__declspec(naked) void kSkyKeyboardHandler()
{
	_asm {
		PUSHAD
		PUSHFD
		CLI
	}

	_asm
	{
		call inthandler21
	}

	SendEOI();

	_asm
	{
		POPFD
		POPAD
		IRETD
	}
}

extern char hankaku[4096];

SkyGUI::SkyGUI()
{
	m_pPressedSheet = nullptr;
	m_RButtonPressed = false;
	m_debugProcessId = -1;
}


SkyGUI::~SkyGUI()
{
}

bool SkyGUI::Print(char* pMsg)
{
	int len = strlen(pMsg);

	if (len <= 0 || len > 256)
		return false;

	if (m_pRenderer && m_mainSheet)
	{
		/*m_pRenderer->PutFontAscToSheet(sht_back, xPos, yPos, COL8_FFFFFF, COL8_000000, pMsg, len);

		yPos += 16;
		xPos = 8;

		if (yPos >= 600)
			yPos = 0;*/
		//len = 5;
		char* pMessage = new char[len + 1];
		memset(pMessage, 0, len + 1);
		memcpy(pMessage, pMsg, len);
		//memcpy(pMessage, "aaaaa", 5);
		//delete pMessage;
		kEnterCriticalSection();
		SendToMessage(m_debugProcessId, pMessage);
		kLeaveCriticalSection();
		/*kEnterCriticalSection();
		SendToMessage(m_debugProcessId, pMessage);
		kLeaveCriticalSection();*/
	}

	return true;
}

bool SkyGUI::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
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

	return true;
}

bool SkyGUI::MakeInitScreen()
{
	m_pRenderer = new SkyRenderer8();
	m_pRenderer->Initialize();

	m_mainSheet = new SkySheetController8();
	m_mainSheet->Initialize((unsigned char*)m_pVideoRamPtr, m_width, m_height);;

	//백그라운드 쉬트를 생성
	sht_back = m_mainSheet->Alloc();
	unsigned char* buf_back = m_mainSheet->AllocBuffer(m_width, m_height);

	sht_back->SetBuf(buf_back, m_width, m_height, -1);
	sht_back->m_movable = false;
	m_pRenderer->InitScreen((unsigned char *)buf_back, m_width, m_height);

	//마우스 쉬트를 생성
	sht_mouse = m_mainSheet->Alloc();
	unsigned char* buf_mouse = m_mainSheet->AllocBuffer(16, 16);
	sht_mouse->SetBuf(buf_mouse, 16, 16, 99);
	sht_mouse->m_movable = false;
	m_pRenderer->InitMouseCursor((char *)buf_mouse, 99);


	mx = (m_width - 16) / 2;
	my = (m_height - 28 - 16) / 2;

	sht_back->Slide(0, 0);
	sht_mouse->Slide(mx, my);

	sht_back->Updown(0);
	sht_mouse->Updown(1);

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
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
bool SkyGUI::MakeIOSystem()
{
	//키보드, 마우스 핸들러 변경
	kEnterCriticalSection();

	fifo32_init(&fifo, 128, fifobuf);
	fifo32_init(&keycmd, 32, keycmd_buf);

	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);

	setvect(0x21, kSkyKeyboardHandler);
	setvect(0x2c, kSkyMouseHandler);

	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);

	kLeaveCriticalSection();

	wait_KBC_sendready();
	OutPortByte(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	OutPortByte(PORT_KEYDAT, KBC_MODE);

	/* 마우스 유효 */
	wait_KBC_sendready();
	OutPortByte(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	OutPortByte(PORT_KEYDAT, MOUSECMD_ENABLE);
	/* 잘되면 ACK(0xfa)가 송신되어 온다 */
	mdec.phase = 0; /* 마우스의 0xfa를 기다리고 있는 단계 */

	OutPortByte(PIC0_IMR, 0xf8); /* PIT와 PIC1와 키보드를 허가(11111000) */
	OutPortByte(PIC1_IMR, 0xef); /* 마우스를 허가(11101111) */

	return true;
}

bool SkyGUI::kGetMessage(LPSKY_MSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	lpMsg->_msgId = SKY_MSG_NO_MESSAGE;
	lpMsg->_extra = 0;

	kEnterCriticalSection();
	if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0)
	{
		/* 키보드 컨트롤러에 보낼 데이터가 있으면, 보낸다 */
		keycmd_wait = fifo32_get(&keycmd);
		kLeaveCriticalSection();
		wait_KBC_sendready();
		OutPortByte(PORT_KEYDAT, keycmd_wait);
		//SkySimpleGUI::FillRect8(100, 100, 100, 100, COL8_C6C6C6, 1024, 768);
	}
	else
		kLeaveCriticalSection();

	kEnterCriticalSection();
	if (fifo32_status(&fifo) != 0)
	{
		lpMsg->_msgId = SKY_MSG_MESSAGE;
		lpMsg->_extra = fifo32_get(&fifo);
	}
	else
	{
		//Process* pProcess = ProcessManager::GetInstance()->GetCurrentTask()->m_pParent;
		//if(pProcess != nullptr)
		//	Scheduler::GetInstance()->Yield(pProcess->GetProcessId());
	}

	kLeaveCriticalSection();

	return true;
}

bool SkyGUI::kTranslateAccelerator(HWND hWnd, HANDLE hAccTable, LPSKY_MSG lpMsg)
{
	return lpMsg->_msgId != SKY_MSG_MESSAGE;
}

bool SkyGUI::Run()
{	
	kEnterCriticalSection();
	bool result = CreateGUIDebugProcess();

	if (result == true)
	{
		Print("Debug Console Started!!\n");
	}

	kLeaveCriticalSection();

	SKY_MSG msg;
	while (kGetMessage(&msg, nullptr, 0, 0))
	{
		//if (!kTranslateAccelerator(msg._hwnd, nullptr, &msg))
		{
			kTranslateMessage(&msg);
			kDispatchMessage(&msg);
		}
	}

	return msg._msgId == SKY_MSG_EXIT;
}

bool SkyGUI::kTranslateMessage(const LPSKY_MSG lpMsg)
{

	if (256 <= lpMsg->_extra && lpMsg->_extra <= 511) //키보드 데이터
	{
		lpMsg->_msgId = SKY_MSG_KEYBOARD;
	}
	else if (512 <= lpMsg->_extra && lpMsg->_extra <= 767) //마우스 데이터 처리
	{

		lpMsg->_msgId = SKY_MSG_MOUSE;
	}

	return true;
}

bool SkyGUI::kDispatchMessage(const LPSKY_MSG lpMsg)
{
	return kWndProc(lpMsg->_hwnd, lpMsg->_msgId, lpMsg->_extra, 0);
}

bool CALLBACK SkyGUI::kWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case SKY_MSG_KEYBOARD:
	{
		ProcessKeyboard(wParam);
	}
	break;
	case SKY_MSG_MOUSE:
	{

		ProcessMouse(wParam);
	}
	break;
	case SKY_MSG_EXIT:
		//Not Implemented
		break;
		//default:
			//return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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

	if ('A' <= s[0] && s[0] <= 'Z') {	/* 입력 문자가 알파벳 */
		if (((key_leds & 4) == 0 && key_shift == 0) ||
			((key_leds & 4) != 0 && key_shift != 0)) {
			s[0] += 0x20;	/* 대문자를 소문자에 변환 */
		}
	}
	kEnterCriticalSection();
	if (s[0] != 0)
	{
		SendToMessage(m_pFocusSheet, SKY_MSG_KEYBOARD, s[0] + 256);
	}
	if (value == 256 + 0x0e) // 백 스페이스
	{
		SendToMessage(m_pFocusSheet, SKY_MSG_KEYBOARD, 8 + 256);
	}
	if (value == 256 + 0x1c)// Enter 
	{
		SendToMessage(m_pFocusSheet, SKY_MSG_KEYBOARD, 10 + 256);
	}
	if (value == 256 + 0x2a) {	/* 왼쪽 쉬프트 ON */
		key_shift |= 1;
	}
	if (value == 256 + 0x36) {	/* 오른쪽 쉬프트 ON */
		key_shift |= 2;
	}
	if (value == 256 + 0xaa) {	/* 왼쪽 쉬프트 OFF */
		key_shift &= ~1;
	}
	if (value == 256 + 0xb6) {	/* 오른쪽 쉬프트 OFF */
		key_shift &= ~2;
	}
	if (value == 256 + 0x3a) {	/* CapsLock */
		key_leds ^= 4;
		fifo32_put(&keycmd, KEYCMD_LED);
		fifo32_put(&keycmd, key_leds);
	}
	if (value == 256 + 0x45) {	/* NumLock */
		key_leds ^= 2;
		fifo32_put(&keycmd, KEYCMD_LED);
		fifo32_put(&keycmd, key_leds);
	}
	if (value == 256 + 0x46) {	/* ScrollLock */
		key_leds ^= 1;
		fifo32_put(&keycmd, KEYCMD_LED);
		fifo32_put(&keycmd, key_leds);
	}

	if (value == 256 + 0xfa) {	/* 키보드가 데이터를 무사하게 받았다 */
		keycmd_wait = -1;
	}
	kLeaveCriticalSection();

	if (value == 256 + 0xfe) {	/* 키보드가 데이터를 무사하게 받을 수 없었다 */
		wait_KBC_sendready();
		OutPortByte(PORT_KEYDAT, keycmd_wait);
	}


}

void SkyGUI::ProcessMouse(int value)
{
	if (mouse_decode(&mdec, value - 512) != 0)
	{
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

		kEnterCriticalSection();

		sht_mouse->Slide(mx, my);
		//sht_mouse->Updown(100);

		if ((mdec.btn & 0x01) != 0)  //왼쪽 버튼을 눌렀다면 마우스 바로 아래의 윈도우를 드래그 처리한다.
		{
			if (m_pPressedSheet == nullptr)
				ProcessMouseLButton(mx, my);
		}
		else
		{
			if (m_pPressedSheet)
			{
				m_pPressedSheet->Slide(mx, my);

			}
			m_pPressedSheet = nullptr;
		}

		if ((mdec.btn & 0x02) != 0) //오른쪽 버튼을 눌렀다면 콘솔 프로세스를 생성한다.
		{
			if (m_RButtonPressed == false)
			{
				m_RButtonPressed = true;
				CreateGUIConsoleProcess();
			}
			//SkySimpleGUI::FillRect8(100, 100, 100, 100, COL8_C6C6C6, 1024, 768);
		}
		else
		{
			m_RButtonPressed = false;
		}

		kLeaveCriticalSection();

	}
}

void SkyGUI::ProcessMouseLButton(int x, int y)
{
	SkySheet* pSheet = m_mainSheet->FindSheet(x, y);
	if (pSheet == nullptr)
	{
		m_pFocusSheet = nullptr;
		m_pPressedSheet = nullptr;
		return;
	}

	SendToMessage(pSheet, SKY_MSG_MOUSE, 0);

	m_pPressedSheet = pSheet;
	m_pFocusSheet = pSheet;
}

bool SkyGUI::SendToMessage(SkySheet* pSheet, int message, int value)
{
	if (pSheet == nullptr)
		return false;

	if (pSheet->m_ownerProcess == m_debugProcessId)
		return false;

	map<int, ConsoleIOListener*>::iterator iter = m_mapIOListener.find(pSheet->m_ownerProcess);

	if (iter == m_mapIOListener.end())
		return false;

	//pSheet->Slide(x, y);

	ConsoleIOListener* listener = (*iter).second;
	listener->PushMessage(message, value);

	return true;
}

bool SkyGUI::SendToMessage(int processID, char* pMsg)
{
	Process* pProcess = ProcessManager::GetInstance()->FindProcess(processID);

	if (pProcess == nullptr)
		return false;

	pProcess->AddMessage(pMsg);

	return true;
}

void SkyGUI::CreateGUIConsoleProcess()
{
	Process* pProcess = nullptr;

	kEnterCriticalSection();
	pProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("GUIConsole", ConsoleGUIProc, this, PROCESS_KERNEL);
	if (pProcess != nullptr)
	{
		//콘솔 태스크 쉬트를 구성
		SkySheet* console = m_mainSheet->Alloc();
		unsigned char* buf = m_mainSheet->AllocBuffer(256, 165);
		console->SetBuf(buf, 256, 165, -1);
		m_pRenderer->MakeWindow(buf, 256, 165, "Sky Console", 0);
		m_pRenderer->MakeTextBox(console, 8, 28, 240, 128, COL8_000000);
		console->Slide(32, 4);
		console->Updown(20);
		console->m_ownerProcess = pProcess->GetProcessId();
	}
	kLeaveCriticalSection();
}

bool SkyGUI::CreateGUIDebugProcess()
{
	Process* pProcess = nullptr;
	bool result = false;
	
	pProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("DEBUGGUI", ConsoleDebugGUIProc, this, PROCESS_KERNEL);
	if (pProcess != nullptr)
	{
		//콘솔 태스크 쉬트를 구성
		SkySheet* console = m_mainSheet->Alloc();
		unsigned char* buf = m_mainSheet->AllocBuffer(256, 165);
		console->SetBuf(buf, 256, 165, -1);
		m_pRenderer->MakeWindow(buf, 256, 165, "DEBUG GUI", 0);
		m_pRenderer->MakeTextBox(console, 8, 28, 240, 128, COL8_000000);
		console->Slide(32, 4);
		console->Updown(20);
		console->m_ownerProcess = pProcess->GetProcessId();

		m_debugProcessId = pProcess->GetProcessId();
		result = true;
	}


	return result;
}

void SkyGUI::RegisterIOListener(int processID, ConsoleIOListener* listener)
{
	kEnterCriticalSection();

	m_mapIOListener[processID] = listener;

	kLeaveCriticalSection();
}

SkySheet* SkyGUI::FindSheetByID(int processId)
{
	return m_mainSheet->FindSheetById(processId);
}





//입력창 태스크(task_a) 생성
/*sht_win = shtctl->Alloc();
buf_win = new unsigned char[160 * 52];
sht_win->SetBuf(buf_win, 144, 52, -1);
SkyRenderer::MakeWindow8(buf_win, 144, 52, "task_a", 1);
SkyRenderer::MakeTextBox8(sht_win, 8, 28, 128, 16, COL8_FFFFFF);
*/

//sht_win->Slide(64, 56);
//sht_win->Updown(2);
int cons_newline(int cursor_y, SkySheet *sheet)
{
	int x, y;
	unsigned char* buf = sheet->GetBuf();
	int bxsize = sheet->GetXSize();
	if (cursor_y < 28 + 112) {
		cursor_y += 16; /* 다음 행에 */
	}
	else {
		/* 스크롤 */
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 240; x++) {
				buf[x + y * bxsize] = buf[x + (y + 16) * bxsize];
			}
		}
		for (y = 28 + 112; y < 28 + 128; y++) {
			for (x = 8; x < 8 + 240; x++) {
				buf[x + y * bxsize] = COL8_000000;
			}
		}
		sheet->Refresh(8, 28, 8 + 240, 28 + 128);
	}
	return cursor_y;
}

DWORD WINAPI ConsoleDebugGUIProc(LPVOID parameter)
{
	kEnterCriticalSection();
	Thread* pThread = ProcessManager::GetInstance()->GetCurrentTask();
	Process* pProcess = pThread->m_pParent;

	SkyGUI* pGUI = (SkyGUI*)parameter;
	SkyRenderer* pRenderer = pGUI->GetRenderer();

	SkySheet *sheet = pGUI->FindSheetByID(pProcess->GetProcessId());

	char s[30], cmdline[30];
	int  cursor_x = 16, cursor_y = 28, cursor_c = -1;

	kLeaveCriticalSection();

	cursor_c = COL8_FFFFFF;

	if(sheet == 0 || pProcess == 0 || pThread == 0 || pGUI == 0)
		SkySimpleGUI::FillRect8(100, 100, 100, 100, COL8_C6C6C6, 1024, 768);

	for (;;)
	{
		kEnterCriticalSection();
		
		list<char*>& messages = pProcess->GetMessageList();
		if (messages.size() > 0)
		{
		
			auto iter = messages.begin();
			for (; iter != messages.end(); iter++)
			{
				if (sheet)
				{
					pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, *iter, strlen(*iter));
					cursor_y = cons_newline(cursor_y, sheet);
				}
			}

			iter = messages.begin();
			for (; iter != messages.end(); iter++)
			{
				delete *iter;
			}

			messages.clear();			

			if(sheet)
				sheet->Refresh(cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
		}

		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());

		kLeaveCriticalSection();
		
	}

	return 0;
}

DWORD WINAPI ConsoleGUIProc(LPVOID parameter)
{
	kEnterCriticalSection();
	Thread* pThread = ProcessManager::GetInstance()->GetCurrentTask();
	Process* pProcess = pThread->m_pParent;

	SkyGUI* pGUI = (SkyGUI*)parameter;
	SkyRenderer* pRenderer = pGUI->GetRenderer();
	ConsoleIOListener* listener = new ConsoleIOListener();

	SkySheet *sheet = pGUI->FindSheetByID(pProcess->GetProcessId());

	pGUI->RegisterIOListener(pProcess->GetProcessId(), listener);

	char s[30], cmdline[30];
	int  cursor_x = 16, cursor_y = 28, cursor_c = -1;

	cursor_c = COL8_FFFFFF;

	/* prompt 표시 */
	pRenderer->PutFontAscToSheet(sheet, 8, 28, COL8_FFFFFF, COL8_000000, ">", 1);

	kLeaveCriticalSection();

	for (;;)
	{

		kEnterCriticalSection();
		if (listener->ReadyStatus() == false)
		{
			Scheduler::GetInstance()->Yield(pProcess->GetProcessId());
			kLeaveCriticalSection();
			continue;
		}
		else
		{
			int i = listener->GetStatus();
			if (i <= 1) { /* 커서용 타이머 */
				if (i != 0) {
					//timer_init(timer, &task->fifo, 0); /* 다음은 0을 */
					if (cursor_c >= 0) {
						cursor_c = COL8_FFFFFF;
					}
				}
				else {
					//timer_init(timer, &task->fifo, 1); /* 다음은 1을 */
					if (cursor_c >= 0) {
						cursor_c = COL8_000000;
					}
				}
				//timer_settime(timer, 50);
			}
			if (i == 2) {	/* 커서 ON */
				cursor_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* 커서 OFF */
				pRenderer->BoxFill(sheet->GetBuf(), sheet->GetXSize(), COL8_000000, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
				cursor_c = -1;
			}
			if (256 <= i && i <= 511) { /* 키보드 데이터(태스크 A경유) */
				if (i == 8 + 256) {
					/* 백 스페이스 */
					if (cursor_x > 16) {
						/* 스페이스로 지우고 나서, 커서를 1개 back */
						pRenderer->PutFontAscToSheet(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
						cursor_x -= 8;
					}
				}
				else if (i == 10 + 256) {

					/* Enter */
					/* 커서를 스페이스에서 지우고 나서 개행한다 */
					pRenderer->PutFontAscToSheet(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
					cmdline[cursor_x / 8 - 2] = 0;
					cursor_y = cons_newline(cursor_y, sheet);

					if (strcmp(cmdline, "mem") == 0)
					{
						size_t totalMemory = PhysicalMemoryManager::GetMemorySize();
						sprintf(s, "total   %dMB", totalMemory / (1024 * 1024));
						pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						sprintf(s, "free %dKB", PhysicalMemoryManager::GetFreeMemory() / 1024);
						pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						cursor_y = cons_newline(cursor_y, sheet);
					}
					else if (strcmp(cmdline, "cls") == 0) /* cls 커맨드 */
					{

						unsigned char* buf = sheet->GetBuf();
						int bxsize = sheet->GetXSize();
						for (int y = 28; y < 28 + 128; y++) {
							for (int x = 8; x < 8 + 240; x++) {
								buf[x + y * bxsize] = COL8_000000;
							}
						}
						sheet->Refresh(8, 28, 8 + 240, 28 + 128);
						cursor_y = 28;
					}

					else if (cmdline[0] != 0) {
						/* 커멘드도 아니고 빈 행도 아니다 */
						pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "Bad command.", 12);
						cursor_y = cons_newline(cursor_y, sheet);
					}
					/* prompt 표시 */
					pRenderer->PutFontAscToSheet(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, ">", 1);
					cursor_x = 16;
				}
				else {

					/* 일반 문자 */
					if (cursor_x < 240) {
						/* 한 글자 표시하고 나서 커서를 1개 진행한다 */
						s[0] = i - 256;
						s[1] = 0;
						cmdline[cursor_x / 8 - 2] = i - 256;
						pRenderer->PutFontAscToSheet(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, s, 1);
						cursor_x += 8;
					}

				}
			}
			/* 커서재표시 */
			if (cursor_c >= 0) {
				pRenderer->BoxFill(sheet->GetBuf(), sheet->GetXSize(), cursor_c, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
			}
			sheet->Refresh(cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
		}

		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());

		kLeaveCriticalSection();

		
	}

	return 0;
}