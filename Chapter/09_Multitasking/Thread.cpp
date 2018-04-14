#include "Thread.h"

Thread::Thread()
{
	m_waitingTime = TASK_RUNNING_TIME;
	m_pParent = NULL;
}


Thread::~Thread()
{
}
