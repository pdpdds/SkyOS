/* FIFO 라이브러리 */

#include "fifo.h"

#define FLAGS_OVERRUN		0x0001

void fifo32_init(FIFO32 *fifo, int size, int *buf)
/* FIFO 버퍼의 초기화 */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* 빈 영역 */
	fifo->flags = 0;
	fifo->p = 0; /* write 위치 */
	fifo->q = 0; /* read 위치 */
	//fifo->task = task; /* 데이터가 들어갔을 때에 일으키는 태스크 */
	return;
}

int fifo32_put(FIFO32 *fifo, int data)
/* FIFO에 데이터를 보내 쌓는다 */
{
	if (fifo->free == 0) {
		/* 빈 영역이 없어서 넘쳤다 */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	
	return 0;
}

int fifo32_get(FIFO32 *fifo)
/* FIFO로부터 데이터를 1개 가져온다 */
{
	int data;
	if (fifo->free == fifo->size) {
		/* 버퍼가 비었을 때는 우선 -1이 주어진다 */
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo32_status(FIFO32 *fifo)
/* 어느 정도 데이터가 모여 있을까를 보고한다 */
{
	if (fifo == nullptr)
		return 0;

	return fifo->size - fifo->free;
}
