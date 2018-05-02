#include "SkyOS.h"

Thread::Thread()
{
	m_waitingTime = TASK_RUNNING_TIME;
	m_pParent = nullptr;
	m_lpTLS = nullptr;
}


Thread::~Thread()
{
}
