#include "UserAPI.h"
#include "string.h"

int main() 
{

	char* message = "Hello world!!\n";
	printf(message);

	/*int first = GetTickCount();
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
	}*/
	

	char* a = new char[100];
	strcpy(a, "Process2 Reply\n");

	printf(a);

	return 0;
	
}
