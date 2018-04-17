#include "KernelProcedure.h"
#include "SkyConsole.h"
#include "string.h"
#include "memory.h"
#include "Hal.h"
#include "PIT.h"
#include "Process.h"
#include "ProcessManager.h"
#include "SkyAPI.h"
#include "ConsoleManager.h"

extern bool systemOn;

void NativeConsole()
{
	systemOn = true;
		
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);	

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
	}
}

DWORD WINAPI SystemConsoleProc(LPVOID parameter)
{	
	SkyConsole::Print("Console Mode Start!!\n");
		
	NativeConsole();

	SkyConsole::Print("Bye!!");

	for (;;);

	return 0;
}