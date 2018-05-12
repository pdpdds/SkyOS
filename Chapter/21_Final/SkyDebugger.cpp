#include "SkyDebugger.h"
#include "SkyConsole.h"
#include "ProcessManager.h"
#include "Thread.h"

SkyDebugger* SkyDebugger::m_pDebugger = nullptr;

SkyDebugger::SkyDebugger()
{
}


SkyDebugger::~SkyDebugger()
{
}

void SkyDebugger::TraceStack(unsigned int maxFrames)
{
	// Stack contains:
	//  Second function argument
	//  First function argument (MaxFrames)
	//  Return address in calling function
	//  EBP of calling function (pointed to by current EBP)
	unsigned int* ebp = &maxFrames - 2;
	SkyConsole::Print("Stack trace:\n");

	Thread* pTask = ProcessManager::GetInstance()->GetCurrentTask();

	

	for (unsigned int frame = 0; frame < maxFrames; ++frame)
	{
		unsigned int eip = ebp[1];
		if (eip == 0)
			// No caller on stack
			break;
		// Unwind to previous stack frame
		ebp = reinterpret_cast<unsigned int *>(ebp[0]);

		unsigned int * arguments = &ebp[2];
		SkyConsole::Print("  0x{%x}     \n", eip);
	}
}