#include "ConsoleIOListener.h"

ConsoleIOListener::ConsoleIOListener()
{
	fifo32_init(&fifo, 128, fifobuf);
}


ConsoleIOListener::~ConsoleIOListener()
{
}

//주의!! 메인 커널 스레드에서만 호출
void ConsoleIOListener::PushMessage(int message, int value)
{
	fifo32_put(&fifo, value);
}

bool ConsoleIOListener::ReadyStatus()
{
	if (fifo32_status(&fifo) == 0)
		return false;

	return true;
}

int ConsoleIOListener::GetStatus()
{
	int i = fifo32_get(&fifo);
	return i;
}
