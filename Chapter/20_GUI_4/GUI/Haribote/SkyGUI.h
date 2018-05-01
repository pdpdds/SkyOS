#pragma once
#include "SkyWindow.h"
#include "windef.h"
#include "SkyMouse.h"
#include "fifo.h"
#include "map.h"

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define KEYCMD_LED		0xed

class SkySheetController;
class SkySheet;
class SkyRenderer;
class ConsoleIOListener;

#define SKY_MSG_NO_MESSAGE 0
#define SKY_MSG_MESSAGE    1
#define SKY_MSG_EXIT       2

#define SKY_MSG_KEYBOARD          3
#define SKY_MSG_MOUSE			  4
#define SKY_MSG_PROCESS_INIT	  5

typedef struct tag_SKY_MSG
{
	int _msgId;
	int _extra;
	HWND _hwnd;

	tag_SKY_MSG()
	{
		_msgId = SKY_MSG_NO_MESSAGE;
		_extra = 0;
	}

}SKY_MSG, *LPSKY_MSG;

class SkyGUI : public SkyWindow
{
public:
	SkyGUI();
	~SkyGUI();
	
	virtual bool Initialize(void* pVideoRamPtr, int width, int height, int bpp) override;
	virtual bool Run() override;

	void RegisterIOListener(int processID, ConsoleIOListener* listener);
	SkyRenderer* GetRenderer() { return m_pRenderer; }
	SkySheet* FindSheetByID(int processId);

protected:
	bool LoadFontFromMemory();

	bool MakeInitScreen();
	bool MakeIOSystem();

	void ProcessKeyboard(int value);
	void ProcessMouse(int value);

	bool kGetMessage(LPSKY_MSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
	bool kTranslateAccelerator(HWND hWnd, HANDLE hAccTable, LPSKY_MSG lpMsg);
	bool kTranslateMessage(const LPSKY_MSG lpMsg);
	bool kDispatchMessage(const LPSKY_MSG lpMsg);
	bool CALLBACK kWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void ProcessMouseLButton(int x, int y);
	void CreateGUIConsoleProcess();
	bool SendToMessage(SkySheet* pSheet, int message, int value);

private:
	ULONG* m_pVideoRamPtr;
	int m_width;
	int m_height;
	int m_bpp;

	map<int, ConsoleIOListener*> m_mapIOListener;

	SkySheetController* m_mainSheet;
	SkyRenderer* m_pRenderer;
	
	SkySheet *sht_back;
	SkySheet *sht_mouse;

	SkySheet* m_pPressedSheet;
	SkySheet* m_pFocusSheet;

	int mx;
	int  my;

	MOUSE_DEC mdec;
	FIFO32 fifo;
	FIFO32 keycmd;
	int fifobuf[128];
	int keycmd_buf[32];

	bool m_RButtonPressed;
	
	int key_to = 0, key_shift = 0, key_leds = 0, keycmd_wait = -1;
	char s[30], cmdline[30];
};