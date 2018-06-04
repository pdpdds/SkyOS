#pragma once
#include "windef.h"
#include "list.h"
#include "map.h"
#include "VirtualMemoryManager.h"

#define PROC_INVALID_ID -1

using namespace std;

class Thread;

extern void HaltSystem(const char* errMsg);

class Process
{	
public:
	Process();
	virtual ~Process();

	typedef map<int, Thread*> ThreadList;

	bool AddMainThread(Thread* pThread); //메인스레드를 추가한다.
	bool AddThread(Thread* pThread); //스레드를 추가한다.

	Thread* GetThreadById(int index); //스레드 아이디로 스레드 객체를 얻는다.
	Thread* GetMainThread(); //스레드 아이디로 스레드 객체를 얻는다.
	
	char		m_processName[MAX_PROCESS_NAME]; //프로세스 이름

	UINT32		m_dwRunState; //프로세스 상태
	UINT32		m_dwPriority; //프로세스 우선순위
	int			m_dwRunningTime; // 프로세스 CPU 선점 시간

	UINT32		m_dwPageCount; //프로세스가 차지하는 페이지 수
	UINT32		m_dwProcessType; //프로세스 타입		
		
	int			m_stackIndex; //스레드가 생성될때 마다 새 스택을 할당해 주어야 하는데 그때 사용되는 인덱스
	ThreadList m_threadList; //프로세스내에 속한 스레드 리스트

	uint32_t	m_imageBase; //파일로 부터 코드를 로드할 경우 사용됨. 메모리에 로드된 주소
	uint32_t	m_imageSize; //파일의 크기

	int			m_mainThreadId;
	bool		m_IskernelProcess;

	void*		m_lpHeap; //유저 프로세스가 사용하는 힙

//페이지 디렉토리를 설정한다.
	PageDirectory* GetPageDirectory() { return m_pPageDirectory; }
	void SetPageDirectory(PageDirectory* pPageDirectory);
	
	int GetProcessId() {return m_processId;}
	void SetProcessId(int processId) { m_processId = processId; }

	bool AddMessage(char* pMsg);
	list<char*>& GetMessageList() { return m_messageList; }

private:
	PageDirectory * m_pPageDirectory; //이	프로세스가 사용하는 페이지 디렉토리		
	int		m_processId; //프로세스 아이디
	list<char*> m_messageList;
	
};