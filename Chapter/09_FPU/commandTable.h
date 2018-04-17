#pragma once

#include "commandMacros.h"	
#include "commands.h"		

START_COMMAND_TABLE

	CMD_ENTRY("q", false, NULL, "quits and halts the system")				
	CMD_ENTRY("help", false, NULL, "Help me please")
	CMD_ENTRY("cls", false, clsCmd, "Clear screen")
	CMD_ENTRY("test", false, cmdTestCPlusPlus, "Test C++ Class")
	CMD_ENTRY("memstate", false, cmdMemState, "Print memory state")
	CMD_ENTRY("testmemtask", false, cmdTesttask, "test memory task")
	CMD_ENTRY("process", false, cmdProcessList, "Print process list")
	CMD_ENTRY("kill", true, cmdKillTask, "Kill process with process id")
	CMD_ENTRY("read", true, cmdRead, "reads a file")
	CMD_ENTRY("gui", false, cmdGUI, "jump to graphic user interface")
	CMD_ENTRY("beep", false, cmdBeep, "test beep sound")

END_COMMAND_TABLE

