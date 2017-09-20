#include "sysapi.h"
#include "string.h"

void CreateHeap()
{
	__asm {

		mov eax, 4
		int 0x80
	}
}

int printf(const char* szMsg)
{
	__asm {

		/* display message through kernel terminal */
		mov ebx, szMsg
		mov eax, 0
		int 0x80
	}
}

void EntryPoint() {

	
	

	char* message = "Hello world!!\n";
	printf(message);

	int first = GetTickCount();
	int count = 4;
	
	while (count != 0)
	{
		//	int b = 0;
		int second = GetTickCount();
		if (second - first > 500)
		{
			printf(message);

			first = GetTickCount();
			count -= 1;
		}
	}

	while (1)
	{

	}

	CreateHeap();

	

	char* a = new char[100];
	strcpy(a, "Process2 Reply\n");

	printf(a);

	

	//for (;;);	

	delete a;
	
	__asm {
			/* terminate */
			mov eax, 1
			int 0x80
	}	
}
