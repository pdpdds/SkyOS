#pragma once
#include "SkyWindow.h"
#include "windef.h"
#include "mouse.h"
#include "fifo.h"

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

class SkyGUI : public SkyWindow
{
public:
	SkyGUI();
	~SkyGUI();
	
	virtual bool Initialize(void* pVideoRamPtr, int width, int height, int bpp) override;
	virtual bool Run() override;

	bool LoadFontFromMemory();

protected:
	bool MakeInitScreen();
	bool MakeIOSystem();

	void ProcessKeyboard(int value);
	void ProcessMouse(int value);

private:
	ULONG* m_pVideoRamPtr;
	int m_width;
	int m_height;
	int m_bpp;

	SkySheetController* shtctl;
	
	unsigned char *buf_back;
	SkySheet *sht_back;


	unsigned char buf_mouse[256];

	unsigned char *buf_win;
	SkySheet *sht_win;
	SkySheet *sht_mouse;

	int mx;
	int  my;
	int  cursor_x;
	int  cursor_c;

	MOUSE_DEC mdec;
	FIFO32 fifo;
	FIFO32 keycmd;
	int fifobuf[128];
	int keycmd_buf[32];

	

	unsigned char* buf_cons;
	SkySheet *sht_cons;
	FIFO32 consoleFifo;

	int key_to = 0, key_shift = 0, key_leds = 0, keycmd_wait = -1;
	char s[30], cmdline[30];
};