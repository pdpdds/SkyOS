#include "Process.h"
#include "Thread.h"

Process::Process()
{
	m_processId = -1;	

	m_dwRunningTime = TASK_RUNNING_TIME;	
	m_pPageDirectory = NULL;
	m_kernelStackIndex = 0;
}

Process::~Process()
{
}

BOOL Process::AddThread(Thread* pThread)
{
	ListNode* node = new ListNode();
	node->_data = pThread;

	return m_threadList.AddToTail(node) != NULL;	
}

Thread* Process::GetThread(int index)
{
	if (index > m_threadList.CountItems())
		return NULL;
	
	if(index == 0)
		return (Thread*)m_threadList.GetHead()->_data;

	ListNode* pNode = m_threadList.GetHead();
	
	while (index > 0)
	{
		pNode = m_threadList.GetNext(pNode);
		index--;
	}

	return (Thread*)pNode->_data;
}


extern void install_pagedirectory(void* pPageDirectory);

void Process::SetPDBR()
{
	install_pagedirectory(m_pPageDirectory);
}