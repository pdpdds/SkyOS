#include "Process.h"
#include "Thread.h"
#include "SkyAPI.h"

Process::Process()
{
	m_processId = 0;	

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
	return m_threadList.insert(std::make_pair(pThread->m_threadId, pThread));	
}

bool Process::AddThread(Thread* pThread)
{	
	return m_threadList.insert(std::make_pair(pThread->m_threadId, pThread));
}

Thread* Process::GetThreadById(int threadId)
{
	if (m_threadList.empty())
		return nullptr;

	ThreadList::iterator iter = m_threadList.find(threadId);
	
	if (iter == m_threadList.end())
		return nullptr;

	return iter->second;
}

Thread* Process::GetMainThread()
{
	ThreadList::iterator iter = m_threadList.find(m_mainThreadId);

	if (iter == m_threadList.end())
		return nullptr;

	return iter->second;
}

void Process::SetPageDirectory(PageDirectory* pPageDirectory)
{			
	M_Assert(pPageDirectory != nullptr, "PageDirectory Is Null.");

	m_pPageDirectory = pPageDirectory;
}