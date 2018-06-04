#include "SkyOS.h"

Process::Process()
{
	m_processId = PROC_INVALID_ID;

	m_dwRunningTime = TASK_RUNNING_TIME;	
	m_pPageDirectory = NULL;
	m_stackIndex = 0;
	m_mainThreadId = -1;

	m_IskernelProcess = true;

	m_lpHeap = nullptr;
}

Process::~Process()
{
}

bool Process::AddMainThread(Thread* pThread)
{
	m_mainThreadId = pThread->GetThreadId();
	m_threadList[pThread->GetThreadId()] = pThread;
	return true;
}

bool Process::AddThread(Thread* pThread)
{	
	m_threadList[pThread->GetThreadId()] = pThread;
	return true;
}

Thread* Process::GetThreadById(int threadId)
{
	if (m_threadList.size() == 0)
		return nullptr;

	ThreadList::iterator iter = m_threadList.find(threadId);
	
	if (iter == m_threadList.end())
		return nullptr;

	return (*iter).second;
}

Thread* Process::GetMainThread()
{
	ThreadList::iterator iter = m_threadList.find(m_mainThreadId);

	if (iter == m_threadList.end())
		return nullptr;

	return (*iter).second;
}

void Process::SetPageDirectory(PageDirectory* pPageDirectory)
{			
	SKY_ASSERT(pPageDirectory != nullptr, "PageDirectory Is Null.");

	m_pPageDirectory = pPageDirectory;
}

bool Process::AddMessage(char* pMsg)
{
	if (pMsg == nullptr)
		return false;

	if (m_messageList.size() > 10)
		return false;

	m_messageList.push_back(pMsg);

	return true;
}