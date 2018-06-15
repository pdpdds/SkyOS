#pragma once

#include "commandMacros.h"	
#include "commands.h"		

START_COMMAND_TABLE
	
	CMD_ENTRY("exit", false, NULL, "Turn off System")
	CMD_ENTRY("help", false, NULL, "Help me please")
	CMD_ENTRY("cls", false, CmdCls, "Clear screen")
	CMD_ENTRY("kill", true, CmdKill, "Kill Process")
	CMD_ENTRY("process", false, CmdProcessList, "List Process")
	CMD_ENTRY("memstate", false, cmdMemState, "Print memory state")
	CMD_ENTRY("watchdog", false, cmdCreateWatchdogTask, "Create watchdog process")
	CMD_ENTRY("taskcount", false, cmdTaskCount, "Get current task count")
	CMD_ENTRY("globalstate", false, cmdGlobalState, "Get memory layout")
	CMD_ENTRY("exec", true, CmdExec, "Execute File")
	CMD_ENTRY("gui", false, cmdGUI, "Get Resolution Mode")	
	CMD_ENTRY("pci", false, cmdPCI, "Get PCI List")
	CMD_ENTRY("dir", false, cmdDir, "Get File List")
	CMD_ENTRY("cd", true, cmdCD, "Change Drive")
	CMD_ENTRY("lua3", true, cmdLua3, "exec lua3 file")
	CMD_ENTRY("lua5", true, cmdLua5, "test lua5 file")
	CMD_ENTRY("guiconsole", false, cmdSwitchGUI, "Switch to GUI Console Mode")	
	CMD_ENTRY("callstack", false, cmdCallStack, "Trace Callstack with Process Id")
	CMD_ENTRY("jpeg", true, cmdJpeg, "Print jpeg")	
END_COMMAND_TABLE

