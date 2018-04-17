#pragma once

struct FIFO32 {
	int *buf;
	int p, q, size, free, flags;	
};

//void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
void fifo32_init(struct FIFO32 *fifo, int size, int *buf);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);
