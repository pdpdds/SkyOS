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

	FIFO32 m_fifo;
	int    m_fifobuf[128];
};

