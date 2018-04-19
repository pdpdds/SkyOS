#include "Scheduler.h"
#include "SkyConsole.h"
#include "PhysicalMemoryManager.h"
#include "ProcessManager.h"
#include "Process.h"
#include "Thread.h"
#include "Hal.h"
#include "sysapi.h"
#include "string.h"
#include "memory.h"

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
	static int count = 0;
	count++;
	if (count % 20 == 0)
	{

		//SkyConsole::Print("TestProc %d\n", count);
	}

				
	ProcessManager::TaskList* pTaskList = ProcessManager::GetInstance()->GetTaskList();

	int taskCount = pTaskList->size();

	if (taskCount == 0)
		HaltSystem("Task Count Is 0\n");

	if (taskCount == 1)
		return true;

	ProcessManager::TaskList::iterator iter = pTaskList->begin();

	Thread* pThread = *iter;

	pThread->m_waitingTime--;
	
	if (pThread->m_waitingTime > 0)
	{
		
		g_pageDirectory = (uint32_t)pThread->m_pParent->GetPageDirectory();
		VirtualMemoryManager::SetCurPageDirectory(pThread->m_pParent->GetPageDirectory());
		return true;
	}

	pThread->m_taskState = TASK_STATE_WAIT;
	pThread->m_contextSnapshot = registers;
	pThread->m_esp = g_esp;

	if (count % 500 == 0)
	{
		//SkyConsole::Print("\n%s, %s %d, %d\n", __FILE__, __func__, __LINE__, pTaskList->size());
	}
	
	pTaskList->remove(pThread);
	pTaskList->push_back(pThread);	

	Thread* pNextThread = pTaskList->front();

	Process* pProcess = pNextThread->m_pParent;

	if (pNextThread->m_taskState == TASK_STATE_INIT)
	{		

		//SkyConsole::Print("%s Initializing\n", pProcess->m_processName);
		
		pNextThread->m_waitingTime = TASK_RUNNING_TIME;
		pNextThread->m_taskState = TASK_STATE_RUNNING;

		entryPoint = (int)pNextThread->frame.eip;
		procStack = pNextThread->frame.esp;
		startParam = pNextThread->m_startParam;

		PageDirectory* pageDirectory = pNextThread->m_pParent->GetPageDirectory();
		VirtualMemoryManager::SetCurPageDirectory(pageDirectory);
		ProcessManager::GetInstance()->SetCurrentTask(pNextThread);
		{

			_asm
			{				
				mov	eax, [pageDirectory]
				mov	cr3, eax	 	// PDBR is cr3 register in i86

				mov ecx, [entryPoint]
				mov esp, procStack
				mov ebx, [startParam]
			}
			
			__asm
			{
				mov     ax, 0x10;
				mov     ds, ax
				mov     es, ax
				mov     fs, ax
				mov     gs, ax			

				push    ebx;
				push    0x10;
				push    0x200; EFLAGS
				push    0x08; CS
				push    ecx; EIP

				mov al, 0x20
				out 0x20, al
				sti

				iretd
			}
		}		
	}
	else
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

		/*if (strcmp(pProcess->m_processName, "ConsoleSystem") == 0)
		{			
			SkyConsole::Print("ConsoleSystem Switch\n"); 

			SkyConsole::Print("%d\n", pNextThread->m_pParent->GetPageDirectory());
			
		}
		if (strcmp(pProcess->m_processName, "WatchDog") == 0)
		{
			SkyConsole::Print("WatchDog Switch\n");
			//while (1);
		}*/


		pNextThread->m_waitingTime = TASK_RUNNING_TIME;
		pNextThread->m_taskState = TASK_STATE_RUNNING;

		g_esp = pNextThread->m_esp;
		
		g_pageDirectory = (uint32_t)pNextThread->m_pParent->GetPageDirectory();

		VirtualMemoryManager::SetCurPageDirectory(pNextThread->m_pParent->GetPageDirectory());
		ProcessManager::GetInstance()->SetCurrentTask(pNextThread);

		g_registers = pNextThread->m_contextSnapshot;

		/*__asm
		{
			
			

			SkyConsole::Print("EDI : %x\n", pNextThread->m_regs.edi);
			SkyConsole::Print("ESI : %x\n", pNextThread->m_regs.esi);
			SkyConsole::Print("EBP : %x\n", pNextThread->m_regs.ebp);
			SkyConsole::Print("ESP : %x\n", pNextThread->m_regs.esp);
			
			mov	edx, g_registers.edx
			mov	ecx, g_registers.ecx
			mov	ebx, g_registers.ebx
			mov	eax, g_registers.eax
			


			SkyConsole::Print("EIP : %x\n", pNextThread->m_regs.eip);
			SkyConsole::Print("CS : %x\n", pNextThread->m_regs.cs);
			SkyConsole::Print("EFLAGS : %x\n", pNextThread->m_regs.eflags);

			SkyConsole::Print("ds : %x\n", pNextThread->m_regs.ds);
			SkyConsole::Print("gs : %x\n", pNextThread->m_regs.gs);
			SkyConsole::Print("es : %x\n", pNextThread->m_regs.es);
			SkyConsole::Print("fs : %x\n", pNextThread->m_regs.fs);
		}*/

	}

	return true;
}