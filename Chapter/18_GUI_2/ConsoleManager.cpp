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

	if (0 == strlen(a_szCommand))
		return FALSE;

	bool result = false;
	for (i = 0; i<g_NumberOfCommands; i++)
	{
		if (g_Commands[i].bHasArguments)
		{
			// Copy the command.
			strcpy(szCmdCopy, a_szCommand);

			if (g_Commands[i].ProcessingFunc != nullptr)
			{
				// This returns the command sub-string.
				pCurrentToken = strtok(szCmdCopy, szDelim);

				if (0 == stricmp(pCurrentToken, g_Commands[i].szCommand))
				{
					pCurrentToken = strtok(NULL, szDelim);

					if (pCurrentToken != nullptr)
					{
						g_Commands[i].ProcessingFunc(pCurrentToken);
						result = true;
						return result;
					}
					else
					{
						SkyConsole::Print("Argument insufficient\n");
						return false;
					}

				}				
			}
		}
		else
		{
			if (g_Commands[i].ProcessingFunc != nullptr)
			{
				// Since we have no arguments, we do an exact match.
				if (0 == stricmp(a_szCommand, g_Commands[i].szCommand))
				{

					g_Commands[i].ProcessingFunc(NULL);
					result = true;
					return result;

				}
			}
		}
	}
	
	if (result == false)
	{
		for (i = 0; i < g_NumberOfCommands; i++)
		{
			if (g_Commands[i].ProcessingFunc == nullptr)
			{
				if (0 == strcmp("help", a_szCommand))
				{
					showHelp();
					return true;
				}
			}			
		}
	}
	
	if (result == false)
	{
		SkyConsole::Print("Command not found....\n");
	}

	return result;
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
		return false;
	}

	if (strcmp(buf, "exit") == 0) 
	{
		return true;
	}	

	processCommandLine(buf);

	return false;
}

