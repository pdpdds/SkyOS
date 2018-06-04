#pragma once
#include "fifo.h"

class ConsoleIOListener
{
public:
	ConsoleIOListener();
	~ConsoleIOListener();

	void PushMessage(int message, int value);
	bool ReadyStatus();
	int GetStatus();

	FIFO32 fifo;
	int    fifobuf[128];
};

