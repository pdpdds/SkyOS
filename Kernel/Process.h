#pragma once
#include "windef.h"
#include "VirtualMemoryManager.h"
#include "kheap.h"
#include "DoubleLinkedList.h"

class Thread;

class Process
{
public:
	Process();
	virtual ~Process();

	BOOL AddThread(Thread* pThread); //스레드를 추가한다.
	Thread* GetThread(int index); //스레드를 삭제한다.

	void SetPDBR();

	UINT32		m_processId; //프로세스 아이디
	char		m_processName[256]; //프로세스 이름
	UINT32		m_dwRunState; //프로세스 상태
	UINT32		m_dwPriority; //프로세스 우선순위
	int			m_dwRunningTime; // 프로세스 CPU 선점 시간
	UINT32		m_dwPageCount; //프로세스가 차지하는 페이지 수
	UINT32		m_dwProcessType; //프로세스 타입	

	PageDirectory* m_pPageDirectory; //이 프로세스가 사용하는 페이지 디렉토리	
	void*		m_lpHeap; //프로세스가 사용하는 힙
		
	int			m_kernelStackIndex; //스레드가 생성될때 마다 새 스택을 할당해 주어야 하는데 그때 사용되는 인덱스
	DoubleLinkedList m_threadList; //프로세스내에 속한 스레드 리스트

	uint32_t	m_imageBase; //파일로 부터 코드를 로드할 경우 사용됨. 파일에 메모리에 로드된 주소
	uint32_t	m_imageSize; //파일의 크기

private:
	
};

