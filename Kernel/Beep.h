/*
	Generates Beep on the PC Speaker		
*/
#pragma once
#include "windef.h"
#include "stdint.h"
#include "Hal.h"

void Sound(uint32_t nFrequence);
void NoSound();
void Beep();