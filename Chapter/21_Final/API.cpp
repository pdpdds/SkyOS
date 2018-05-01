#include "API.h"
#include "SkyStruct.h"
#include "SkyConsole.h"

struct APIStruct
{
	char * strAPIName;		//function name
	void * ptrAPIFunction;		//function pointer
	struct APIStruct * Next;
};
struct APIStruct * RegisteredAPIEntries = 0;

void RegisterAPI(char * strAPIName, void * ptrAPIFunction)
{
	struct APIStruct * NewAPIStruct;
	char * strName;
	SkyConsole::Print("Reg : %s %x\n", strAPIName, ptrAPIFunction);
	//allocating the required space
	NewAPIStruct = (struct APIStruct *) new struct APIStruct;
	strName = new char[strlen(strAPIName) + 1];

	strcpy(strName, strAPIName);

	//assiging the values
	NewAPIStruct->strAPIName = strName;
	NewAPIStruct->ptrAPIFunction = ptrAPIFunction;
	NewAPIStruct->Next = 0;	//terminating the linked list

	if (RegisteredAPIEntries)//if root exist find  the last node and add to it
	{
		struct APIStruct * CurAPIStruct = RegisteredAPIEntries;
		while (CurAPIStruct->Next)
		{
			if (strcmp(CurAPIStruct->strAPIName, strAPIName) == 0) //already the function exists update the pointer value
				break;
			CurAPIStruct = CurAPIStruct->Next;
		}
		CurAPIStruct->Next = NewAPIStruct;
	}
	else
		RegisteredAPIEntries = NewAPIStruct; // creating node
}
void * GetAPIEntryPoint(char * strAPIName)
{
	struct APIStruct * CurAPIStruct = RegisteredAPIEntries;
	while (CurAPIStruct)
	{
		SkyConsole::Print("[%s] [%s]\n", strAPIName, CurAPIStruct->strAPIName);
		if (strcmp(CurAPIStruct->strAPIName, strAPIName) == 0)
			return CurAPIStruct->ptrAPIFunction;
		CurAPIStruct = CurAPIStruct->Next;
	}
	return 0;
}
