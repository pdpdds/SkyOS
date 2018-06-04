#include "SkyMouse.h"
#include "Hal.h"
#include "fifo.h"
#include "SkyGUI.h"
#include "SkyGUIConsole.h"

FIFO32 *mousefifo = nullptr;
int mousedata0 = 0;

void ProcessSkyMouseHandler()
{
	int data;
	OutPortByte(PIC1_OCW2, 0x64);	/* IRQ-12 접수 완료를 PIC1에 통지 */
	OutPortByte(PIC0_OCW2, 0x62);	/* IRQ-02 접수 완료를 PIC0에 통지 */
	data = InPortByte(PORT_KEYDAT);
	int result = fifo32_put(mousefifo, data + mousedata0);

	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void EnableMouse(FIFO32 *fifo, int data0, MOUSE_DEC *mdec)
{
	/* write할 FIFO 버퍼를 기억 */
	mousefifo = fifo;
	mousedata0 = data0;
	
	return;
}

int DecodeMouseValue(MOUSE_DEC *mdec, unsigned char dat)
{
	if (mdec->phase == 0) {
		/* 마우스의 0xfa를 기다리고 있는 단계 */
		if (dat == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) {
		/* 마우스의 1바이트째를 기다리고 있는 단계 */
		if ((dat & 0xc8) == 0x08) {
			/* 올바른 1바이트째였다 */
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2) {
		/* 마우스의 2바이트째를 기다리고 있는 단계 */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		/* 마우스의 3바이트째를 기다리고 있는 단계 */
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; /* 마우스에서는 y방향의 부호가 화면과 반대 */
		return 1;
	}

	//SkyGUI::FillRect8(100, 100, 100, 100, 8, 1024, 768);
	return -1; /* 여기에 올 일은 없을 것 */
}


FIFO32 *keyfifo;
int keydata0;

void ProcessSkyKeyboardHandler()
{
	int data;
	OutPortByte(PIC0_OCW2, 0x61);	/* IRQ-01 접수 완료를 PIC에 통지 */
	data = InPortByte(PORT_KEYDAT);
	fifo32_put(keyfifo, data + keydata0);
	return;
}

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void wait_KBC_sendready(void)
{
	/* 키보드 컨트롤러가 데이터 송신이 가능하게 되는 것을 기다린다 */
	for (;;) {
		if ((InPortByte(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}


void init_keyboard(FIFO32 *fifo, int data0)
{
	/* write할 FIFO 버퍼를 기억 */
	keyfifo = fifo;
	keydata0 = data0;
	/* 키보드컨트롤러의 초기화 */
	
	return;
}