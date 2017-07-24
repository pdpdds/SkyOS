#include "sysapi.h"
#include "string.h"

void CreateHeap()
{
	__asm {

		mov eax, 4
		int 0x80
	}
}

int printf(const char* str)
{
	__asm {

		/* display message through kernel terminal */
		mov ebx, str
		mov eax, 0
		int 0x80
	}
}

void EntryPoint() {

	CreateHeap();

	char* message ="Hello world!!\n";	
	printf(message);

	char* a = new char[100];
	strcpy(a, "Process2 Reply\n");

	printf(a);

	int first = GetTickCount();
	int count = 4;
	while (count != 0)
	{
	//	int b = 0;
		int second = GetTickCount();
		if (second - first > 500)
		{
			printf(a);

			first = GetTickCount();
			//count -= 1;
		}
	}

	//for (;;);	

	delete a;
	
	__asm {
			/* terminate */
			mov eax, 1
			int 0x80
	}	
}
