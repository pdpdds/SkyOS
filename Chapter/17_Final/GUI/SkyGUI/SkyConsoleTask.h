#pragma once
#include "windef.h"
#include "SkyGUI.h"

class Thread;
class SkyRenderer;
class ConsoleIOListener;
class SkySheet;

class SkyConsoleTask
{
public:
	SkyConsoleTask();
	~SkyConsoleTask();

	bool Init(SkyGUI* pGUI, Thread* pTask);
	bool Run();

	int GetNewLine();

	bool kGetMessage(LPSKY_MSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
	bool kTranslateAccelerator(HWND hWnd, HANDLE hAccTable, LPSKY_MSG lpMsg);
	bool kTranslateMessage(const LPSKY_MSG lpMsg);
	bool kDispatchMessage(const LPSKY_MSG lpMsg);
	bool CALLBACK kWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ProcessKeyboard(int value);

private:
	SkyRenderer* m_pRenderer;
	SkyGUI* m_pGUI;
	ConsoleIOListener* m_pIOListener;
	SkySheet* m_pSheet;
	int   m_ProcessId;

	int m_cursorX;
	int m_cursorY;
	int m_cursorCol;

	char s[30], cmdline[30];
};

