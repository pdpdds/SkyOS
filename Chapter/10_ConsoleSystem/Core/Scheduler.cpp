#include "SkyOS.h"

Scheduler* Scheduler::m_pScheduler = 0;
extern bool systemOn;
registers_t g_registers;

uint32_t lastTickCount = 0;
extern int g_esp;
extern uint32_t g_pageDirectory;
extern uint32_t g_pageDirectory1;

int entryPoint = 0;
unsigned int procStack = 0;
LPVOID startParam = 0;

void SwitchTask(int tick, registers_t& registers)
{
	if (systemOn == false)
		return;

	Scheduler::GetInstance()->DoSchedule(tick, registers);
}

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
}

bool  Scheduler::DoSchedule(int tick, registers_t& registers)
{

#ifdef _ORANGE_DEBUG
	/*uint32_t currentTickCount = GetTickCount();

	if (currentTickCount - lastTickCount > 300)
	{
		SkyConsole::Print("\nSwitch Stack Report\n");

		SkyConsole::Print("EDI : %x\n", registers.edi);
		SkyConsole::Print("ESI : %x\n", registers.esi);
		SkyConsole::Print("EBP : %x\n", registers.ebp);
		SkyConsole::Print("ESP : %x\n", registers.esp);
		SkyConsole::Print("EBX : %x\n", registers.ebx);
		SkyConsole::Print("EDX : %x\n", registers.edx);
		SkyConsole::Print("ECX : %x\n", registers.ecx);
		SkyConsole::Print("EAX : %x\n", registers.eax);

		SkyConsole::Print("EIP : %x\n", registers.eip);
		SkyConsole::Print("CS : %x\n", registers.cs);
		SkyConsole::Print("EFLAGS : %x\n", registers.eflags);
		SkyConsole::Print("USERESP : %x\n", registers.useresp);
		SkyConsole::Print("SS : %x\n", registers.ss);

		lastTickCount = currentTickCount;
	}*/
#endif	
	//태스크 리스트를 얻어온다
	ProcessManager::TaskList* pTaskList = ProcessManager::GetInstance()->GetTaskList();

	int taskCount = pTaskList->size();

	if (taskCount == 0)
		HaltSystem("Task Count Is 0\n");

	//태스크가 하나뿐이라면 스케쥴링을 할 필요가 없다.
	if (taskCount == 1)
		return true;

	ProcessManager::TaskList::iterator iter = pTaskList->begin();

	Thread* pThread = *iter;

	pThread->m_waitingTime--;
	
	//태스크 리스트에서 선두에 있는 태스크가 현재 수행되고 있는 태스크다.
	//수행시간이 아직 남아 있으면 태스크 스위칭을 하지 않는다.
	if (pThread->m_waitingTime > 0)
	{
		
		g_pageDirectory = (uint32_t)pThread->m_pParent->GetPageDirectory();
		VirtualMemoryManager::SetCurPageDirectory(pThread->m_pParent->GetPageDirectory());
		return true;
	}

	//태스크의 수행시간이 만료되었으면 태스크의 상태를 대기상태로 변경하고
	//현재 태스크의 레지스터값들을 스냅샷한다.
	pThread->m_taskState = TASK_STATE_WAIT;
	pThread->m_contextSnapshot = registers;
	pThread->m_esp = g_esp;
	
	//SkyConsole::Print("\n%s, %s %d, %d\n", __FILE__, __func__, __LINE__, pTaskList->size());
	
	//만료된 태스크를 리스트 뒷쪽에 넣는다.
	pTaskList->remove(pThread);
	pTaskList->push_back(pThread);	

	Thread* pNextThread = pTaskList->front();

	Process* pProcess = pNextThread->m_pParent;

	//처음으로 시작되는 태스크인가?
	if (pNextThread->m_taskState == TASK_STATE_INIT)
	{		
		
		//태스크의 상태를 수행중으로 변경하고 수행시간을 확보한다.
		pNextThread->m_waitingTime = TASK_RUNNING_TIME;
		pNextThread->m_taskState = TASK_STATE_RUNNING;

		//태스크의 엔트리 포인터와 스택주소, 파라메터값을 얻는다.
		entryPoint = (int)pNextThread->frame.eip;
		procStack = pNextThread->frame.esp;
		startParam = pNextThread->m_startParam;

		//페이지 디렉토리를 얻어온다.
		PageDirectory* pageDirectory = pNextThread->m_pParent->GetPageDirectory();
		VirtualMemoryManager::SetCurPageDirectory(pageDirectory);
		ProcessManager::GetInstance()->SetCurrentTask(pNextThread);
		{

			_asm
			{				
				mov	eax, [pageDirectory]
				mov	cr3, eax	 	// CR3 레지스터에 페이지 디렉토리 설정
				//페이지가 변경되어도 entryPoint procStack startParam은 커널영역에 선언된 변수라
				//문제없이 사용가능하다.
				mov ecx, [entryPoint]
				mov esp, procStack
				xor ebp, ebp
				push ebp
				mov ebx, [startParam]
			}
			
			__asm
			{
				mov     ax, 0x10;데이터 세그먼트 셀렉터 초기화
				mov     ds, ax
				mov     es, ax
				mov     fs, ax
				mov     gs, ax			

				push    ebx;
				push    0; //EBP
				push    0x200; EFLAGS
				push    0x08; CS
				push    ecx; EIP
				//인터럽트 활성화후 새 태스크를 실행한다.
				mov al, 0x20
				out 0x20, al
				sti

				iretd
			}
		}		
	}
	else//이미 실행중인 태스크인 경우
	{
#ifdef _ORANGE_DEBUG
		/*SkyConsole::Print("EDI : %x\n", pNextThread->m_regs.edi);
		SkyConsole::Print("ESI : %x\n", pNextThread->m_regs.esi);
		SkyConsole::Print("EBP : %x\n", pNextThread->m_regs.ebp);
		SkyConsole::Print("ESP : %x\n", pNextThread->m_regs.esp);
		SkyConsole::Print("EBX : %x\n", pNextThread->m_regs.ebx);
		SkyConsole::Print("EDX : %x\n", pNextThread->m_regs.edx);
		SkyConsole::Print("ECX : %x\n", pNextThread->m_regs.ecx);
		SkyConsole::Print("EAX : %x\n", pNextThread->m_regs.eax);

		SkyConsole::Print("EIP : %x\n", pNextThread->m_regs.eip);
		SkyConsole::Print("CS : %x\n", pNextThread->m_regs.cs);
		SkyConsole::Print("EFLAGS : %x\n", pNextThread->m_regs.eflags);

		SkyConsole::Print("ds : %x\n", pNextThread->m_regs.ds);
		SkyConsole::Print("gs : %x\n", pNextThread->m_regs.gs);
		SkyConsole::Print("es : %x\n", pNextThread->m_regs.es);
		SkyConsole::Print("fs : %x\n", pNextThread->m_regs.fs);*/
#endif		

		//태스크의 ESP와 페이지 디렉토리 값을 g_esp, g_pageDirectory에 설정
		pNextThread->m_waitingTime = TASK_RUNNING_TIME;
		pNextThread->m_taskState = TASK_STATE_RUNNING;

		g_esp = pNextThread->m_esp;
		
		g_pageDirectory = (uint32_t)pNextThread->m_pParent->GetPageDirectory();

		VirtualMemoryManager::SetCurPageDirectory(pNextThread->m_pParent->GetPageDirectory());
		ProcessManager::GetInstance()->SetCurrentTask(pNextThread);

		//레지스터
		g_registers = pNextThread->m_contextSnapshot;		
	}

	return true;
}

bool Scheduler::Yield(int processId)
{
	kEnterCriticalSection();

	ProcessManager::TaskList* pTaskList = ProcessManager::GetInstance()->GetTaskList();
	ProcessManager::TaskList::iterator iter = pTaskList->begin();

	for (; iter != pTaskList->end(); iter++)
	{
		Thread* pThread = *iter;
		if (pThread->m_pParent->GetProcessId() == processId)
			pThread->m_waitingTime = 0;
	}

	kLeaveCriticalSection();

	return true;
	
}