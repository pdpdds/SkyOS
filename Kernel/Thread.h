#pragma once
#include "windef.h"
#include "task.h"
#include "Hal.h"

class Process;

class Thread
{
public:
	Thread();
	virtual ~Thread();

//스레드 로컬 스토리지
	void*		m_lpTLS = NULL;
	
	int			m_taskState; //태스크 상태. Init, Running, Stop, Terminate
	UINT32		m_dwPriority; //우선순위
	int			m_waitingTime; // 태스크 CPU 선점 시간	

	Process*	m_pParent = NULL; //부모 프로세스

	LPVOID		m_startParam; //스레드 시작시 제공되는 파라메터
	void*		m_initialStack; //베이스 스택 주소
	uint32_t	m_esp;	//태스크의 스택 포인터
	UINT32		m_stackLimit; //스택의 크기
	void*		kernelStack;		
	trapFrame	frame;

	uint32_t	m_imageBase;  // 파일에서 코드를 로드할 경우, 파일이 메모리에 로드된 베이스 주소
	uint32_t	m_imageSize;  //파일의 크기. 파일을 로드해서 생성되는 스레드가 아닌 경우 m_imageBase와 m_imageSize는 의미가 없다.

	
	registers_t m_contextSnapshot; //태스크의 컨텍스트 스냅샷(레지스터)
};