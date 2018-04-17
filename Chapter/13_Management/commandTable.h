#pragma once

#include "commandMacros.h"	
#include "commands.h"		

START_COMMAND_TABLE
	
	CMD_ENTRY("exit", false, NULL, "Turn off System")
	CMD_ENTRY("help", false, NULL, "Help me please")
	CMD_ENTRY("cls", false, CmdCls, "Clear screen")
	CMD_ENTRY("kill", true, CmdKill, "Kill Process")
	CMD_ENTRY("process", false, CmdProcessList, "List Process")

END_COMMAND_TABLE

