#include "HariboteGUI.h"
#include "windef.h"
#include "sprintf.h"
#include "SkyAPI.h"
#include "mouse.h"
#include "fifo.h"
#include "ProcessManager.h"
#include "PhysicalMemoryManager.h"
#include "SkySimpleGUI.h"

#define KEYCMD_LED		0xed

static char closebtn[14][16] = {
	{'O','O', 'O','O','O','O','O','O','O','O','O','O','O','O','O','@' },
{ 'O','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','$','@' },
{ 'O','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','$','@' },
{ 'O','Q','Q','Q','@','@','Q','Q','Q','Q','@','@','Q','Q','$','@' },
{ 'O','Q','Q','Q','Q','@','@','Q','Q','@','@','Q','Q','Q','$','@' },
{ 'O','Q','Q','Q','Q','Q','@','@','@','@','Q','Q','Q','Q','$','@' },
{ 'O','Q','Q','Q','Q','Q','Q','@','@','Q','Q','Q','Q','Q','$','@' },
{ 'O','Q','Q','Q','Q','Q','@','@','@','@','Q','Q','Q','Q','$','@' },
{ 'O','Q','Q','Q','Q','@','@','Q','Q','@','@','Q','Q','Q','$','@' },
{ 'O','Q','Q','Q','@','@','Q','Q','Q','Q','@','@','Q','Q','$','@' },
{ 'O','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','$','@' },
{ 'O','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','Q','$','@' },
{ 'O','$','$','$','$','$','$','$','$','$','$','$','$','$','$','@' },
{ '@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@' }
};

static char cursor[16][16] = {
	{'*','*','*','*','*','*','*','*','*','*','*','*','*','*','.','.'},
{'*','O','O','O','O','O','O','O','O','O','O','O','*','.','.','.'},
{'*','O','O','O','O','O','O','O','O','O','O','*','.','.','.','.'},
{'*','O','O','O','O','O','O','O','O','O','*','.','.','.','.','.'},
{'*','O','O','O','O','O','O','O','O','*','.','.','.','.','.','.'},
{'*','O','O','O','O','O','O','O','*','.','.','.','.','.','.','.'},
{'*','O','O','O','O','O','O','O','*','.','.','.','.','.','.','.'},
{'*','O','O','O','O','O','O','O','O','*','.','.','.','.','.','.'},
{'*','O','O','O','O','*','*','O','O','O','*','.','.','.','.','.'},
{'*','O','O','O','*','.','.','*','O','O','O','*','.','.','.','.'},
{'*','O','O','*','.','.','.','.','*','O','O','O','*','.','.','.'},
{'*','O','*','.','.','.','.','.','.','*','O','O','O','*','.','.'},
{'*','*','.','.','.','.','.','.','.','.','*','O','O','O','*','.'},
{'*','.','.','.','.','.','.','.','.','.','.','*','O','O','O','*'},
{'.','.','.','.','.','.','.','.','.','.','.','.','*','O','O','*'},
{'.','.','.','.','.','.','.','.','.','.','.','.','.','*','*','*'}
};

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

int key_to = 0, key_shift = 0, key_leds = 0, keycmd_wait = -1;
char s[30], cmdline[30];

int cons_newline(int cursor_y, struct SHEET *sheet)
{
	int x, y;
	if (cursor_y < 28 + 112) {
		cursor_y += 16; /* 다음 행에 */
	}
	else {
		/* 스크롤 */
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
			}
		}
		for (y = 28 + 112; y < 28 + 128; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	}
	return cursor_y;
}

DWORD WINAPI console_task(LPVOID parameter)
{
	HariboteGUI* pGUI = (HariboteGUI*)parameter;
	SHEET *sheet = pGUI->GetConsoleSheet();

	struct TIMER *timer;
	//struct TASK *task = task_now();
	int i, fifobuf[128], cursor_x = 16, cursor_y = 28, cursor_c = -1;
	char s[30], cmdline[30];	
	int x, y;
	//struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);

	
	//timer = timer_alloc();
	//timer_init(timer, &task->fifo, 1);
	//timer_settime(timer, 50);

	/* prompt 표시 */
	pGUI->putfonts8_asc_sht(sheet, 8, 28, COL8_FFFFFF, COL8_000000, ">", 1);
	
	for (;;) {
		kEnterCriticalSection();
		if (fifo32_status(pGUI->GetConsoleFifo()) == 0) {
			//task_sleep(task);			
			kLeaveCriticalSection();
		}
		else {			
			i = fifo32_get(pGUI->GetConsoleFifo());
			kLeaveCriticalSection();			
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
				pGUI->boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
				cursor_c = -1;
			}
			if (256 <= i && i <= 511) { /* 키보드 데이터(태스크 A경유) */				
				if (i == 8 + 256) {
					/* 백 스페이스 */
					if (cursor_x > 16) {
						/* 스페이스로 지우고 나서, 커서를 1개 back */
						pGUI->putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
						cursor_x -= 8;
					}
				}
				else if (i == 10 + 256) {

					/* Enter */
					/* 커서를 스페이스에서 지우고 나서 개행한다 */
					pGUI->putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
					cmdline[cursor_x / 8 - 2] = 0;
					cursor_y = cons_newline(cursor_y, sheet);
					
					if (strcmp(cmdline, "mem") == 0) 
					{					
						size_t totalMemory = PhysicalMemoryManager::GetMemorySize();
						sprintf(s, "total   %dMB", totalMemory / (1024 * 1024));
						pGUI->putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						sprintf(s, "free %dKB", PhysicalMemoryManager::GetFreeMemory() / 1024);
						pGUI->putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						cursor_y = cons_newline(cursor_y, sheet);
					}
					else if (strcmp(cmdline, "cls") == 0) {
						/* cls 커맨드 */
						for (y = 28; y < 28 + 128; y++) {
							for (x = 8; x < 8 + 240; x++) {
								sheet->buf[x + y * sheet->bxsize] = COL8_000000;
							}
						}
						sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
						cursor_y = 28;
					}
					else if (strcmp(cmdline, "dir") == 0) {
						/* dir 커맨드 */
						/*for (x = 0; x < 224; x++) {
							if (finfo[x].name[0] == 0x00) {
								break;
							}
							if (finfo[x].name[0] != 0xe5) {
								if ((finfo[x].type & 0x18) == 0) {
									sprintf(s, "filename.ext   %7d", finfo[x].size);
									for (y = 0; y < 8; y++) {
										s[y] = finfo[x].name[y];
									}
									s[9] = finfo[x].ext[0];
									s[10] = finfo[x].ext[1];
									s[11] = finfo[x].ext[2];
									putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
									cursor_y = cons_newline(cursor_y, sheet);
								}
							}
						}*/
						//cursor_y = cons_newline(cursor_y, sheet);
					}
					else if (cmdline[0] != 0) {
						/* 커멘드도 아니고 빈 행도 아니다 */
						pGUI->putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "Bad command.", 12);
						cursor_y = cons_newline(cursor_y, sheet);
						cursor_y = cons_newline(cursor_y, sheet);
					}
					/* prompt 표시 */
					pGUI->putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, ">", 1);
					cursor_x = 16;
				}
				else {					
					
					/* 일반 문자 */
					if (cursor_x < 240) {
						/* 한 글자 표시하고 나서 커서를 1개 진행한다 */
						s[0] = i - 256;
						s[1] = 0;
						cmdline[cursor_x / 8 - 2] = i - 256;
						pGUI->putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, s, 1);
						cursor_x += 8;
					}

				}
			}
			/* 커서재표시 */
			if (cursor_c >= 0) {
				pGUI->boxfill8(sheet->buf, sheet->bxsize, cursor_c, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
			}
			sheet_refresh(sheet, cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
		}
	}
}


HariboteGUI::HariboteGUI()
{
}


HariboteGUI::~HariboteGUI()
{
}

void HariboteGUI::boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void HariboteGUI::init_screen8(unsigned char *vram, int x, int y)
{
	boxfill8(vram, x, COL8_008484, 0, 0, x - 1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6, 0, y - 28, x - 1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF, 0, y - 27, x - 1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6, 0, y - 26, x - 1, y - 1);

	boxfill8(vram, x, COL8_FFFFFF, 3, y - 24, 59, y - 24);
	boxfill8(vram, x, COL8_FFFFFF, 2, y - 24, 2, y - 4);
	boxfill8(vram, x, COL8_848484, 3, y - 4, 59, y - 4);
	boxfill8(vram, x, COL8_848484, 59, y - 23, 59, y - 5);
	boxfill8(vram, x, COL8_000000, 2, y - 3, 59, y - 3);
	boxfill8(vram, x, COL8_000000, 60, y - 24, 60, y - 3);

	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x - 4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y - 4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y - 3, x - 4, y - 3);
	boxfill8(vram, x, COL8_FFFFFF, x - 3, y - 24, x - 3, y - 3);
	return;
}

void HariboteGUI::set_palette(int start, int end, unsigned char *rgb)
{
	kEnterCriticalSection();


	for (int i = start; i <= end; i++) {
		OutPortByte(0x03c9, rgb[0] / 4);
		OutPortByte(0x03c9, rgb[1] / 4);
		OutPortByte(0x03c9, rgb[2] / 4);
		rgb += 3;
	}

	kLeaveCriticalSection();

}

void HariboteGUI::InitPalette()
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:흑 */
		0xff, 0x00, 0x00,	/*  1:밝은 빨강 */
		0x00, 0xff, 0x00,	/*  2:밝은 초록 */
		0xff, 0xff, 0x00,	/*  3:밝은 황색 */
		0x00, 0x00, 0xff,	/*  4:밝은 파랑 */
		0xff, 0x00, 0xff,	/*  5:밝은 보라색 */
		0x00, 0xff, 0xff,	/*  6:밝은 물색 */
		0xff, 0xff, 0xff,	/*  7:흰색 */
		0xc6, 0xc6, 0xc6,	/*  8:밝은 회색 */
		0x84, 0x00, 0x00,	/*  9:어두운 빨강 */
		0x00, 0x84, 0x00,	/* 10:어두운 초록 */
		0x84, 0x84, 0x00,	/* 11:어두운 황색 */
		0x00, 0x00, 0x84,	/* 12:어두운 파랑 */
		0x84, 0x00, 0x84,	/* 13:어두운 보라색 */
		0x00, 0x84, 0x84,	/* 14:어두운 물색 */
		0x84, 0x84, 0x84	/* 15:어두운 회색 */
	};
	set_palette(0, 15, table_rgb);
}

void HariboteGUI::putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, d /* data */;
	
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

//char test[16] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,1,1,1,1,1,1,1,1,1};


extern char hankaku[4096];

void HariboteGUI::putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{	
	for (; *s != 0x00; s++) {
	//putfont8(vram, xsize, x, y, c, test);
	putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
	x += 8;
	}
	return;
}

extern void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
extern void inthandler2c();
extern void inthandler21();

__declspec(naked) void kMouseHandler()
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

__declspec(naked) void kKeyboardHandler()
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

bool HariboteGUI::Init(char* vram, int screenX, int screenY)
{
//해상도, 비디오 버퍼 설정
	this->m_screenX = screenX;
	this->m_screenY = screenY;
	this->m_vram = vram;

//키보드, 마우스 핸들러 변경
	kEnterCriticalSection();

	setvect(0x21, kKeyboardHandler);
	setvect(0x2c, kMouseHandler);

	kLeaveCriticalSection();
	
	fifo32_init(&fifo, 128, fifobuf);
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	OutPortByte(PIC0_IMR, 0xf8); /* PIT와 PIC1와 키보드를 허가(11111000) */
	OutPortByte(PIC1_IMR, 0xef); /* 마우스를 허가(11101111) */
	fifo32_init(&keycmd, 32, keycmd_buf);

	fifo32_init(&consoleFifo, 128, fifobuf);
	
	InitPalette();
	shtctl = shtctl_init((unsigned char*)vram, screenX, screenY);
	
//백그라운드 쉬트를 생성
	sht_back = sheet_alloc(shtctl);
	buf_back = new unsigned char[screenX * screenY];
	sheet_setbuf(sht_back, buf_back, screenX, screenY, -1); /* 투명색없음 */
	
	init_screen8((unsigned char *)buf_back, screenX, screenY);
	
//콘솔 태스크 쉬트를 구성
	sht_cons = sheet_alloc(shtctl);
	buf_cons = new unsigned char[256 * 165];
	sheet_setbuf(sht_cons, buf_cons, 256, 165, -1);
	make_window8(buf_cons, 256, 165, "console", 0);
	make_textbox8(sht_cons, 8, 28, 240, 128, COL8_000000);

//입력창 태스크(task_a) 생성
	sht_win = sheet_alloc(shtctl);
	buf_win = new unsigned char[160 * 52];
	sheet_setbuf(sht_win, buf_win, 144, 52, -1); /* 투명색없음 */
	make_window8(buf_win, 144, 52, "task_a", 1);
	make_textbox8(sht_win, 8, 28, 128, 16, COL8_FFFFFF);
	cursor_x = 8;
	cursor_c = COL8_FFFFFF;
	
//마우스 쉬트를 생성
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8((char *)buf_mouse, 99);
	mx = (screenX - 16) / 2;
	my = (screenY - 28 - 16) / 2;
	
	sheet_slide(sht_back,  0,  0);
	sheet_slide(sht_cons, 32,  4);
	sheet_slide(sht_win,  64, 56);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back,  0);
	sheet_updown(sht_cons,  1);
	sheet_updown(sht_win,   2);
	sheet_updown(sht_mouse, 3);	
	
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);
	//kEnterCriticalSection();
	ProcessManager::GetInstance()->CreateProcessFromMemory("Console", console_task, this, PROCESS_KERNEL);
	//kLeaveCriticalSection();
	return true;
}

bool HariboteGUI::Run()
{	

	SkySimpleGUI::FillRect8(100, 100, 100, 100, COL8_C6C6C6, 1024, 768);
	for (;;) {		

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
		else {
			
			
			int i = fifo32_get(&fifo);
			__asm sti
			if (256 <= i && i <= 511) { /* 키보드 데이터 */
				
				if (i < 0x80 + 256) { /* 키코드를 문자 코드로 변환 */
					if (key_shift == 0) {
						s[0] = keytable0[i - 256];
					}
					else {
						s[0] = keytable1[i - 256];
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
				if (s[0] != 0) { /* 통상 문자 */
					if (key_to == 0) {	/* 태스크 A에 */
						if (cursor_x < 128) {
							/* 한 글자 표시하고 나서, 커서를 1개 진행한다 */
							s[1] = 0;
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
							cursor_x += 8;
						}
					}
					else {	/* 콘솔에 */
						//boxfill8(sht_win->buf, sht_win->bxsize, COL8_FF0000, 20, 20, 40, 40);
						fifo32_put(&consoleFifo, s[0] + 256);
					}
				}
				if (i == 256 + 0x0e) {	/* 백 스페이스 */
					if (key_to == 0) {	/* 태스크 A에 */
						if (cursor_x > 8) {
							/* 스페이스로 지우고 나서, 커서를 1개 back */
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
							cursor_x -= 8;
						}
					}
					else {	/* 콘솔에 */
						fifo32_put(&consoleFifo, 8 + 256);
					}
				}
				if (i == 256 + 0x1c) {	/* Enter */
					if (key_to != 0) {	/* 콘솔에 */
						fifo32_put(&consoleFifo, 10 + 256);
					}
				}
				if (i == 256 + 0x0f) {	/* Tab */
					if (key_to == 0) {
						key_to = 1;
						make_wtitle8(buf_win, sht_win->bxsize, "task_a", 0);
						make_wtitle8(buf_cons, sht_cons->bxsize, "console", 1);
						cursor_c = -1; /* 커서를 지운다 */
						boxfill8(sht_win->buf, sht_win->bxsize, COL8_FFFFFF, cursor_x, 28, cursor_x + 7, 43);
						fifo32_put(&consoleFifo, 2); /* 콘솔의 커서 ON */
					}
					else {
						key_to = 0;
						make_wtitle8(buf_win, sht_win->bxsize, "task_a", 1);
						make_wtitle8(buf_cons, sht_cons->bxsize, "console", 0);
						cursor_c = COL8_000000; /* 커서를 낸다 */
						fifo32_put(&consoleFifo, 3); /* 콘솔의 커서 OFF */
					}
					sheet_refresh(sht_win, 0, 0, sht_win->bxsize, 21);
					sheet_refresh(sht_cons, 0, 0, sht_cons->bxsize, 21);
				}
				if (i == 256 + 0x2a) {	/* 왼쪽 쉬프트 ON */
					key_shift |= 1;
				}
				if (i == 256 + 0x36) {	/* 오른쪽 쉬프트 ON */
					key_shift |= 2;
				}
				if (i == 256 + 0xaa) {	/* 왼쪽 쉬프트 OFF */
					key_shift &= ~1;
				}
				if (i == 256 + 0xb6) {	/* 오른쪽 쉬프트 OFF */
					key_shift &= ~2;
				}
				if (i == 256 + 0x3a) {	/* CapsLock */
					key_leds ^= 4;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x45) {	/* NumLock */
					key_leds ^= 2;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x46) {	/* ScrollLock */
					key_leds ^= 1;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0xfa) {	/* 키보드가 데이터를 무사하게 받았다 */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) {	/* 키보드가 데이터를 무사하게 받을 수 없었다 */
					wait_KBC_sendready();
					OutPortByte(PORT_KEYDAT, keycmd_wait);
				}
				/* 커서의 재표시 */
				if (cursor_c >= 0) {
					boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				}
				sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			}
			else if (512 <= i && i <= 767) { /* 마우스 데이터 */		
				//boxfill8((unsigned char*)m_vram, 1024, COL8_FF0000, 520, 0, 560, 20);
				if (mouse_decode(&mdec, i - 512) != 0) {
					/* 마우스 커서의 이동 */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > m_screenX - 1) {
						mx = m_screenX - 1;
					}
					if (my > m_screenY - 1) {
						my = m_screenY - 1;
					}
					sheet_slide(sht_mouse, mx, my);
					if ((mdec.btn & 0x01) != 0) {
						/* 왼쪽 버튼을 누르고 있으면 sht_win를 움직인다 */
						sheet_slide(sht_win, mx - 80, my - 8);
					}
				}
			}
			else if (i <= 1) { /* 커서용 타이머 */
				if (i != 0) {
					//timer_init(timer, &fifo, 0); /* 다음은 0을 */
					if (cursor_c >= 0) {
						cursor_c = COL8_000000;
					}
				}
				else {
					//timer_init(timer, &fifo, 1); /* 다음은 1을 */
					if (cursor_c >= 0) {
						cursor_c = COL8_FFFFFF;
					}
				}
				//timer_settime(timer, 50);
				if (cursor_c >= 0) {
					boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
					sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
				}
			}
		}
	}

	return true;
}

void HariboteGUI::TestRun(uint8_t* dmaVideo)
{
	InitPalette();

	unsigned char* video = (unsigned char*)dmaVideo;

	for (int i = 0; i < 0x0F0000; i++)
	{
		video[i] = COL8_840000;
	}

	boxfill8(video, 1024, COL8_FF0000, 520, 0, 1024, 280);
	boxfill8(video, 1024, COL8_00FF00, 70, 50, 170, 150);
	boxfill8(video, 1024, COL8_0000FF, 120, 80, 220, 180);

	for (;;);
}

/* 마우스 커서를 준비(16 x16) */
void HariboteGUI::init_mouse_cursor8(char *mouse, char bc)
{	
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

void HariboteGUI::make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act)
{
	boxfill8(buf, xsize, COL8_C6C6C6, 0, 0, xsize - 1, 0);
	boxfill8(buf, xsize, COL8_FFFFFF, 1, 1, xsize - 2, 1);
	boxfill8(buf, xsize, COL8_C6C6C6, 0, 0, 0, ysize - 1);
	boxfill8(buf, xsize, COL8_FFFFFF, 1, 1, 1, ysize - 2);
	boxfill8(buf, xsize, COL8_848484, xsize - 2, 1, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, xsize - 1, 0, xsize - 1, ysize - 1);
	boxfill8(buf, xsize, COL8_C6C6C6, 2, 2, xsize - 3, ysize - 3);
	boxfill8(buf, xsize, COL8_848484, 1, ysize - 2, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, 0, ysize - 1, xsize - 1, ysize - 1);
	make_wtitle8(buf, xsize, title, act);
	return;
}

void HariboteGUI::make_wtitle8(unsigned char *buf, int xsize, char *title, char act)
{

	int x, y;
	char c, tc, tbc;
	if (act != 0) {
		tc = COL8_FFFFFF;
		tbc = COL8_000084;
	}
	else {
		tc = COL8_C6C6C6;
		tbc = COL8_848484;
	}
	boxfill8(buf, xsize, tbc, 3, 3, xsize - 4, 20);
	putfonts8_asc((char *)buf, xsize, 24, 4, tc, (unsigned char *)title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL8_000000;
			}
			else if (c == '$') {
				c = COL8_848484;
			}
			else if (c == 'Q') {
				c = COL8_C6C6C6;
			}
			else {
				c = COL8_FFFFFF;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

void HariboteGUI::putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l)
{
	boxfill8(sht->buf, sht->bxsize, b, x, y, x + l * 8 - 1, y + 15);
	putfonts8_asc((char *)sht->buf, sht->bxsize, x, y, c, (unsigned char *)s);
	sheet_refresh(sht, x, y, x + l * 8, y + 16);
	return;
}

void HariboteGUI::make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c)
{
	int x1 = x0 + sx, y1 = y0 + sy;
	boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
	boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
	boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
	boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, c, x0 - 1, y0 - 1, x1 + 0, y1 + 0);
	return;
}
