#pragma once

#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

void wait_KBC_sendready(void);
void init_keyboard(struct FIFO32 *fifo, int data0);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
