#pragma once

typedef struct tag_FIFO32 {
	int *buf;
	int p, q, size, free, flags;
	int message;
}FIFO32;

void fifo32_init(FIFO32 *fifo, int size, int *buf);
int fifo32_put(FIFO32 *fifo, int data);
int fifo32_get(FIFO32 *fifo);
int fifo32_status(FIFO32 *fifo);
