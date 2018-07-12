#include "SkyGUILauncher.h"
#include "SkyConsole.h"
#include "ProcessManager.h"
#include "SkyAPI.h"
#include "KernelProcedure.h"
#include "defines.h"
#include "Exception.h"
#include "Process.h"
#include "Thread.h"

SkyGUILauncher::SkyGUILauncher()
{
}


SkyGUILauncher::~SkyGUILauncher()
{
}

void SkyGUILauncher::Launch()
{
	Process* pMainProcess = ProcessManager::GetInstance()->CreateProcessFromMemory("GUISystem", SystemGUIProc, NULL, PROCESS_KERNEL);
	ProcessManager::GetInstance()->CreateProcessFromMemory("ProcessRemover", ProcessRemoverProc, NULL, PROCESS_KERNEL);	
	ProcessManager::GetInstance()->CreateProcessFromMemory("SystemIdle", SystemIdle, NULL, PROCESS_KERNEL);
	SkyConsole::Print("Init GUI System....\n");

	Thread* pThread = pMainProcess->GetMainThread();
	
	Jump(pThread);
}
