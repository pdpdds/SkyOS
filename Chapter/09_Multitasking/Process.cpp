#include "Process.h"
#include "Thread.h"
#include "SkyAPI.h"

Process::Process()
{
	m_processId = 0;	

	m_dwRunningTime = TASK_RUNNING_TIME;	
	m_pPageDirectory = NULL;
	m_stackIndex = 0;
	m_IskernelProcess = true;
}

Process::~Process()
{
}

bool Process::AddThread(Thread* pThread)
{
	return m_threadList.insert(std::make_pair(pThread->m_threadId, pThread));	
}

Thread* Process::GetThread(int threadId)
{
	if (m_threadList.empty())
		return nullptr;

	ThreadList::iterator iter = m_threadList.begin();
	
	if (iter == m_threadList.end())
		return nullptr;

	return iter->second;
}

void Process::SetPageDirectory(PageDirectory* pPageDirectory)
{			
	M_Assert(pPageDirectory != nullptr, "PageDirectory Is Null.");

	m_pPageDirectory = pPageDirectory;
}