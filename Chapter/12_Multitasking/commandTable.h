#pragma once

#include "commandMacros.h"	
#include "commands.h"		

START_COMMAND_TABLE
	
	CMD_ENTRY("exit", false, NULL, "Turn off System")
	CMD_ENTRY("help", false, NULL, "Help me please")
	CMD_ENTRY("cls", false, clsCmd, "Clear screen")	

END_COMMAND_TABLE

