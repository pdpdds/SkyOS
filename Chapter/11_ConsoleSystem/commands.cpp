#include "SkyConsole.h"
#include "string.h"
#include "memory.h"
#include "stdio.h"
#include "Hal.h"
#include "ProcessManager.h"
#include "ZetPlane.h"
#include "PhysicalMemoryManager.h"
#include "PIT.h"
#include "KernelProcedure.h"
#include "SkyAPI.h"

long clsCmd(char *theCommand)
{
	SkyConsole::Clear();
	return false;
}