#include "kmain.h"


void Test();

//void kmain64(unsigned long magic, unsigned long addr)
void kmain64()
{

	Test();

	for (;;);
	SkyConsole::Initialize();
	
	SkyConsole::Print("64Bit Kernel Entered..\n");

	for (;;);
}

void Test()
{

}