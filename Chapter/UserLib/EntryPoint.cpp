#include "EntryPoint.h"
#include "UserAPI.h"
#include "string.h"

extern int main();

extern "C" void EntryPoint() 
{
	CreateHeap();	
	int result = main();
	
	TerminateProcess();

	for (;;);
}