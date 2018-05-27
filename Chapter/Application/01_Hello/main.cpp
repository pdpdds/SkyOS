#include "UserAPI.h"
#include "string.h"

int main() 
{		
	char* message = "Hello world!!\n";
	printf(message);

	char* reply = new char[100];
	strcpy(reply, "Process Reply\n");

	printf(reply);

	delete reply;

	return 0;	
}
