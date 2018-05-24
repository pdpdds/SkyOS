#pragma once
#include "fifo.h"

#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

typedef struct tag_MOUSE_DEC 
{
	unsigned char buf[3], phase;
	int x, y, btn;
}MOUSE_DEC;

void wait_KBC_sendready(void);
void init_keyboard(FIFO32 *fifo, int data0);
int DecodeMouseValue(MOUSE_DEC *mdec, unsigned char dat);
