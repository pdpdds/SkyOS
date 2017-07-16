#pragma once
#include "StdIntTypes.h"
#include "HardwareSubordination.h"

bool SetupGDT();
bool SetupIDT();
bool SetupPIC();
bool SetupInterruptHandlers();
bool SetupPIT();