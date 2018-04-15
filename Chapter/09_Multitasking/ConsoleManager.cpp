#include "ConsoleManager.h"
#include "string.h"
#include "stdio.h"
#include "SkyConsole.h"
#include "PhysicalMemoryManager.h"
#include "VirtualMemoryManager.h"
#include "ProcessUtil.h"
#include "ProcessManager.h"
#include "ZetPlane.h"
#include "PIT.h"
#include "FloppyDisk.h"
#include "commandTable.h"

void showHelp(void)
{
	SkyConsole::Print("Framework Version: %s\n", gFrameWorkVersion);
	SkyConsole::Print("\n");

	for (ULONG i = 0; i < g_NumberOfCommands; i++)
	{
		if (g_Commands[i].szCommand[0] != '_')
		{
			SkyConsole::Print(("%s : %s\n"), (const char*)(g_Commands[i].szCommand), (const char*)(g_Commands[i].comments));
		}
	}

	SkyConsole::Print("\n");
}

long processCommandLine(char *a_szCommand)
{
	ULONG   i;
	char   szCmdCopy[256];
	char   szDelim[] = " ";
	char  *pCurrentToken;

	// Make sure that there is a command to process.
	if (0 == strlen(a_szCommand))
		return FALSE;

	// Check whether we asked for help.
	if (0 == strcmp(a_szCommand, g_Commands[1].szCommand))
	{
		showHelp();

		return 0;
	}

	// Find if it is any of the supported commands (except QUIT on index 0)
	for (i = 2; i<g_NumberOfCommands; i++)
	{
		if (g_Commands[i].bHasArguments)
		{
			// Copy the command.
			strcpy(szCmdCopy, a_szCommand);

			// This returns the command sub-string.
			pCurrentToken = strtok(szCmdCopy, szDelim);

			if (0 == stricmp(pCurrentToken, g_Commands[i].szCommand))
			{
				pCurrentToken = strtok(NULL, szDelim);
				g_Commands[i].ProcessingFunc(pCurrentToken);
				return 0;
			}
		}
		else
		{
			// Since we have no arguments, we do an exact match.
			if (0 == stricmp(a_szCommand, g_Commands[i].szCommand))
			{
				g_Commands[i].ProcessingFunc(NULL);  // MOD120203: use NULL instead of a_szCommand
				return 0;
			}
		}
	}

	if (i == g_NumberOfCommands)
	{
		SkyConsole::Print("Command not found....\n");
	}

	return FALSE;
}

ConsoleManager::ConsoleManager()
{
}

ConsoleManager::~ConsoleManager()
{
}


bool ConsoleManager::RunCommand(char* buf) 
{
	if (buf[0] == '\0')
	{
		ZetPlane* pPlane = new ZetPlane();
		delete pPlane;
		SkyConsole::Print("dsfds\n");
		return false;
	}

	if (strcmp(buf, "exit") == 0) 
	{
		return true;
	}
	else if (strstr(buf, ".exe") > 0) {

		cmdProc(buf);
		return false;
	}

	processCommandLine(buf);

	return false;
}

