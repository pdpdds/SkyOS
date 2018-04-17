#include "Process.h"
#include "Thread.h"
#include "SkyAPI.h"

Process::Process()
{
	m_processId = PROC_INVALID_ID;

	m_dwRunningTime = TASK_RUNNING_TIME;	
	m_pPageDirectory = NULL;
	m_stackIndex = 0;
	m_mainThreadId = -1;

	m_IskernelProcess = true;
}

Process::~Process()
{
}

bool Process::AddMainThread(Thread* pThread)
{
	m_mainThreadId = pThread->m_threadId;
	return m_threadList.insert(pThread->m_threadId, pThread) != m_threadList.end();
}

bool Process::AddThread(Thread* pThread)
{	
	return m_threadList.insert(pThread->m_threadId, pThread) != m_threadList.end();
}

Thread* Process::GetThreadById(int threadId)
{
	if (m_threadList.size() == 0)
		return nullptr;

	ThreadList::iterator iter = m_threadList.find(threadId);
	
	if (iter == m_threadList.end())
		return nullptr;

	return *iter;
}

Thread* Process::GetMainThread()
{
	ThreadList::iterator iter = m_threadList.find(m_mainThreadId);

	if (iter == m_threadList.end())
		return nullptr;

	return *iter;
}

void Process::SetPageDirectory(PageDirectory* pPageDirectory)
{			
	M_Assert(pPageDirectory != nullptr, "PageDirectory Is Null.");

	m_pPageDirectory = pPageDirectory;
}