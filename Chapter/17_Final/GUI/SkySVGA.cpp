#include "SkySVGA.h"
#include "svgaguientry.h"
#include "VESA.h"
#include "Hal.h"
#include "svga_mouse.h"
#include "svga_keyb.h"
#include "SkyAPI.h"
#include "SkyOS.h"
#include "SkyGUISystem.h"
#include "ProcessManager.h"
#include "Process.h"

extern void kSVGAKeyboardHandler();
extern void kSVGAMouseHandler();
extern int cpp_func(int arg1, int arg2);
lua_State* g_L;
SkySVGA::SkySVGA()
{
	m_exit_program = false;
	m_pWinThread = nullptr;
}

SkySVGA::~SkySVGA()
{
}
#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

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

extern void wait_KBC_sendready(void);
extern void mouse_init();

extern void update_browser(GuiObject * obj);

#include "ConsoleManager.h"
#include "I_Hangul.h"
#include "SkyStartOption.h"
#include "SkyGUISystem.h"

void OutputBrowser(GuiObject * obj, char *pMsg)
{	
	int len = strlen(obj->buffer);

	//백스페이스
	if (strlen(pMsg) == 1 && pMsg[0] == 0x08)
	{
		return;
	}
	unsigned char *s = (unsigned char*)pMsg;
	for (; *s != 0x00; s++)
	{
		if (*s == '\n')
		{
			//GetNewLine();
			continue;
		}

		if (*s == '\r')
		{
			//GetNewLine();
			continue;
		}

		if (!isascii(*s) || (*s) < 0x20)
			continue;

		obj->buffer[len] = *s;
		len++;
	}
	obj->buffer[len++] = '\n';
	obj->buffer[len] = 0;

	set_browser_text(obj, obj->buffer);
	update_browser(obj);
}

extern void TestLua51(lua_State* L);
extern void TestLua52(lua_State* L);
extern void TestLua53(lua_State* L);
extern void TestLua54(lua_State* L);
extern void TestLua55(lua_State* L);
extern void TestLua56(lua_State* L);

void terminal_dispatch(GuiObject * obj, char *command)
{
	/*I_Hangul* pEngine = SkyGUISystem::GetInstance()->GetUnicodeEngine();
	pEngine->PutFonts((char*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr, 1024, 0, 0, 0xffffffff, (unsigned char*)command);
	ConsoleManager cm;
	cm.RunCommand(command);
	strcat(obj->buffer, command);
	strcat(obj->buffer, "\n");
	set_browser_text(obj, obj->buffer);
	update_browser(obj);*/
	if (strcmp(command, "uname3") == 0) {
		//strcat(obj->buffer, "SkyOS 2018 0.12\n");
		//set_browser_text(obj, obj->buffer);
		//update_browser(obj);
		//int result = luatinker::call<int>(g_L, "lua_func", 3, 4);
		//printf("lua_func(3,4) = %d\n", result);
		kEnterCriticalSection();
		memset(obj->buffer, 0, 65535);
		set_browser_text(obj, obj->buffer);
		update_browser(obj);
		g_L = luaL_newstate();
		luaopen_base(g_L);
		TestLua53(g_L);
		lua_close(g_L);
		kLeaveCriticalSection();
	}
	if (strcmp(command, "uname1") == 0) {
		//strcat(obj->buffer, "SkyOS 2018 0.12\n");
		//set_browser_text(obj, obj->buffer);
		//update_browser(obj);
		//int result = luatinker::call<int>(g_L, "lua_func", 3, 4);
		//printf("lua_func(3,4) = %d\n", result);
		kEnterCriticalSection();
		memset(obj->buffer, 0, 65535);
		set_browser_text(obj, obj->buffer);
		update_browser(obj);
		g_L = luaL_newstate();
		luaopen_base(g_L);
		TestLua51(g_L);
		lua_close(g_L);
		kLeaveCriticalSection();
	}
	if (strcmp(command, "uname2") == 0) {
		//strcat(obj->buffer, "SkyOS 2018 0.12\n");
		//set_browser_text(obj, obj->buffer);
		//update_browser(obj);
		//int result = luatinker::call<int>(g_L, "lua_func", 3, 4);
		//printf("lua_func(3,4) = %d\n", result);
		kEnterCriticalSection();
		memset(obj->buffer, 0, 65535);
		set_browser_text(obj, obj->buffer);
		update_browser(obj);
		g_L = luaL_newstate();
		luaopen_base(g_L);
		TestLua52(g_L);
		lua_close(g_L);
		kLeaveCriticalSection();
	}
	if (strcmp(command, "uname4") == 0) {
		//strcat(obj->buffer, "SkyOS 2018 0.12\n");
		//set_browser_text(obj, obj->buffer);
		//update_browser(obj);
		//int result = luatinker::call<int>(g_L, "lua_func", 3, 4);
		//printf("lua_func(3,4) = %d\n", result);
		kEnterCriticalSection();
		memset(obj->buffer, 0, 65535);
		set_browser_text(obj, obj->buffer);
		update_browser(obj);
		g_L = luaL_newstate();
		luaopen_base(g_L);
		TestLua54(g_L);
		lua_close(g_L);
		kLeaveCriticalSection();
	}
	if (strcmp(command, "uname5") == 0) {
		//strcat(obj->buffer, "SkyOS 2018 0.12\n");
		//set_browser_text(obj, obj->buffer);
		//update_browser(obj);
		//int result = luatinker::call<int>(g_L, "lua_func", 3, 4);
		//printf("lua_func(3,4) = %d\n", result);
		kEnterCriticalSection();
		memset(obj->buffer, 0, 65535);
		set_browser_text(obj, obj->buffer);
		update_browser(obj);
		g_L = luaL_newstate();
		luaopen_base(g_L);
		TestLua55(g_L);
		lua_close(g_L);
		kLeaveCriticalSection();
	}	
}

static void terminalinput_cb(GuiObject * obj, int user_data)
{
	GuiWinThread *win_thread = (obj->win)->win_thread;
	GuiWindow *win;
	GuiObject *object;

	win = obj->win;
	object = win->first;

	if (object->objclass == BROWSER) {
		terminal_dispatch(object, obj->label);
		memset(obj->label, 0, strlen(obj->label));
		//OutputBrowser(object, (char*)user_data);
		return;
	}

	while (object->next != NULL) {
		object = object->next;
		if (object->objclass == BROWSER) {
			terminal_dispatch(object, obj->label);
			memset(obj->label, 0, strlen(obj->label));
			return;
		}
	}
}

static GuiWindow *create_terminal(GuiWinThread *win_thread)
{
	GuiWindow *win;
	GuiObject *browser, *text;

	win = add_window(win_thread, NORMAL_WINDOW, (1024 - 640) / 2, (768 - 480) / 2, 640, 480, "System Terminal", FALSE, FALSE);
	browser = add_browser(win, 5, 20, 630, 430, VERT_SLIDER);
	set_browser_text(browser, "SkyOS System Terminal initialized.\n");
	text = add_input(win, NORMAL_INPUT, 5, 480 - 20, 625, 256);
	set_object_callback(text, terminalinput_cb);	
	create_window(win);

	return win;
}

static void endtask_cb(GuiObject * obj, int data)
{
	show_button(obj);
	if (question_dialog((obj->win)->win_thread, NULL, "Do you want to end this task?", DIA_QUESTION)) {
		message_dialog((obj->win)->win_thread, NULL, "You cannot end this task!", DIA_INFO);
	}
}



static void threadsview_listbox_cb(GuiObject * obj, int data)
{
	//fprintf(stderr, "Listbox: Testing %d pressed\n", data);
}


static void threadsview_listbox_cb2(GuiObject * obj, int data)
{
	//fprintf(stderr, "Listbox: Testing %d double clicked\n", data);
}

static GuiWindow * create_threadsview_win(GuiWinThread * win_thread)
{
	GuiWindow *win;
	GuiObject *obj, *listbox;
	ProcessManager::ProcessList* pProcessList = ProcessManager::GetInstance()->GetProcessList();
	int k;

	win = add_window(win_thread, NORMAL_WINDOW, (1024 - 300) / 2, (768 - 500) / 2, 300, 500, "SkyOS ThreadsView", FALSE, FALSE);
	obj = add_button(win, NORMAL_BUTTON, 10, 30, 80, 17, "Applications");
	obj = add_button(win, NORMAL_BUTTON, 95, 30, 80, 17, "Performance");
	obj = add_button(win, NORMAL_BUTTON, 180, 30, 80, 17, "Networking");
	obj = add_text(win, NORMAL_TEXT, 5, 60, "Taskname");
	listbox = add_listbox(win, 10, 80, 300 - 35, 25);
	k = 0;

	auto iter = pProcessList->begin();
	for (; iter != pProcessList->end(); iter++)
	{
		k++;
		Process* pProcess = (*iter).second;
		obj = add_listentry(listbox, pProcess->m_processName);
		set_object_callback(obj, threadsview_listbox_cb);
		set_object_callback2(obj, threadsview_listbox_cb2);
		set_object_user_data(obj, k);
	}

	create_listbox(listbox);
	obj = add_button(win, NORMAL_BUTTON, 200, 450, 80, 20, "End Task");
	set_object_callback(obj, endtask_cb);
	create_window(win);

	return win;
}

bool SkySVGA::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
{	
	init_lfb();
	//lfb_clear();

	kEnterCriticalSection();
	SetInterruptVector(0x21, kSVGAKeyboardHandler);
	SetInterruptVector(0x2c, kSVGAMouseHandler);

	
	//luatinker::def(g_L, "cpp_func", cpp_func);
	//luatinker::dofile(g_L, "sample1.lua");
	
	kLeaveCriticalSection();

	mouse_init();
	return true;
}

bool SkySVGA::Run()
{		
	StartGui();
	
	for (;;);

	return false;
}

bool SkySVGA::Print(char* pMsg)
{
	//do_windows(m_pWinThread);

	if (strlen(pMsg) < 2)
		return false;

	if (m_pWinThread == nullptr)
		return false;

	GuiWindow* pWindow = m_pWinThread->oldfocuswin;
	if (pWindow == NULL)
		return false;
		
	GuiObject* input = pWindow->inputfocus;
	if (input == NULL)
		return false;

	GuiObject *object;
	object = pWindow->first;

	if (object == nullptr)
		return false;

	if (object->objclass == BROWSER) {
		//terminal_dispatch(object, strdup(obj->label));
		//memset(obj->label, 0, strlen(obj->label));
		OutputBrowser(object, (char*)pMsg);
		//HaltSystem("sfdssdffds");
		return true;
	}

	while (object->next != NULL) {
		object = object->next;
		if (object->objclass == BROWSER) {
			OutputBrowser(object, (char*)pMsg);
			//HaltSystem("sfdssdffds");
			return true;
		}
	}

	//input->object_callback(input, (int)pMsg);

	return true;
}

bool SkySVGA::Clear()
{
	return false;
}



int SkySVGA::StartGui()
{	
	GuiObject *obj = NULL;
	int type = SVGALIB;

	//GUI 모드 초기화
	init_svgagui();

	//1024*768 32비트로 초기화한다. 
	open_screen(type, 1024, 768, 256, "SVGAGui");
	update_screen();

	//마우스 이미지만 초기화
	init_mouse();
	kleur = BACKGROUND;

	//윈도우 컴포넌트를 담을 수 있는 윈도우 쉬트를 생성한다.
	m_pWinThread = create_window_thread();

	//샘플 윈도우 컴포넌트를 윈도우 쉬트에 추가한다.
	//StartSampleGui();
	create_terminal(m_pWinThread);
	create_threadsview_win(m_pWinThread);

	//갱신된 버퍼를 화면에 출력한다.
	show_window_thread(m_pWinThread);
	//int result = luatinker::call<int>(g_L, "lua_func", 3, 4);
	//printf("lua_func(3,4) = %d\n", result);
	//luaL_dostring(g_L, "");

	//입력 처리가 구현되어 있지 않으므로 루프에서 벗어나지 않는다.
	while (!m_exit_program) {
		obj = do_windows(m_pWinThread);
		if (obj == (obj->win)->kill)
			delete_window(obj->win, TRUE);
	}

	delete_window_thread(m_pWinThread);
	close_screen();

	return 0;
}
