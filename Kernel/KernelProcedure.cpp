#include "KernelProcedure.h"
#include "ConsoleManager.h"
#include "Console.h"
#include "string.h"
#include "Hal.h"
#include "PIT.h"
#include "Process.h"

bool systemOn = false;

void NativeConsole()
{
	systemOn = true;
	ConsoleManager manager;

	char	commandBuffer[MAXPATH];

	while (1)
	{
		SkyConsole::Print("Command> ");
		memset(commandBuffer, 0, MAXPATH);
		//SkyConsole::Print("commandBuffer Address : 0x%x\n", &commandBuffer);	

		SkyConsole::GetCommand(commandBuffer, MAXPATH - 2);
		SkyConsole::Print("\n");

		if (manager.RunCommand(commandBuffer) == true)
			break;

		/*int first = GetTickCount();
		int count = 4;
		while (count != 0)
		{

		int second = GetTickCount();
		if (second - first > 100)
		{
		SkyConsole::Print("%d\n", second);

		first = GetTickCount();
		}
		}*/
	}
}

DWORD WINAPI SystemEntry(LPVOID parameter)
{	
	while (1) {
		NativeConsole();
	}

	for (;;);

	return 0;
}

DWORD WINAPI SampleLoop(LPVOID parameter)
{
	char* str = "Sample Loop!!\n";
	SkyConsole::Print("%s", str);
	int first = GetTickCount();
	bool bExit = false;
	SkyConsole::Print("%s", str);	

	Process* pProcess = (Process*)parameter;
	SkyConsole::Print("Thread Routine Process Address %x\n", pProcess);

	while (bExit == false)
	{
		static int count = 0;
		int second = GetTickCount();
		if (second - first > 100)
		{
			SkyConsole::Print("%s", str);

			first = GetTickCount();
			count++;
		}
	}

	return 0;
}

DWORD WINAPI TaskProcessor(LPVOID parameter)
{
	Process* pProcess = (Process*)parameter;
	SkyConsole::Print("TaskProcessor Thread Parent Process Address %x\n", pProcess);

	while (true)
	{
	//	simpl
	}

	return 0;
}

DWORD WINAPI TestKernelProcess(LPVOID parameter)
{		
	Process* pProcess = (Process*)parameter;
	SkyConsole::Print("Test Second Kernel Process %x\n", pProcess);
	BOOL bExit = false;
	int first = GetTickCount();

	while (bExit == false)
	{
		static int count = 0;
		
		int second = GetTickCount();
		if (second - first > 100)
		{
			SkyConsole::Print("Test Second Kernel Process %x\n", pProcess);

			first = GetTickCount();
			count++;
		}
	}

	return 0;
}
