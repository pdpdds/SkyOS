#pragma once
#include "stdint.h"
#include "defines.h"
#include "string.h"
#include "sprintf.h"

//extern "C" void kmain64(unsigned long, unsigned long);
extern "C" void kmain64(int addr);
extern bool InitializeConstructors();

