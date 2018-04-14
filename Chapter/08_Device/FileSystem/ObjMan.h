#pragma once
#include "windef.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define OBJECT_MESSAGE_BOX_SIZE 100
struct Object
{
	char szName[20]; //name of the object
	UINT32 dwUses;           //total uses
    
   // LPTASK   lpTask;          //Owner Task            (these fields are automatically created)
   // LPTHREAD lpThread;        //Owner Thread ID
    
    //LPMESSAGEBOX lpMessageBox;      //Message Box
	
    UINT32 dwSignals;       //Signal state
    
	PVOID lpIData;          //Internal data
};

typedef struct Object OBJECT;
typedef OBJECT * LPOBJECT;

struct WaitForObject
{
    LPOBJECT lpWaitForObject;   //waiting for the object
   // LPTASK lpTask;                //waiting Task
   // LPTHREAD lpThread;            //waiting Thread
};
typedef struct WaitForObject WAITFOROBJECT;
typedef WAITFOROBJECT * LPWAITFOROBJECT;


extern struct _ListNode * sysObjects;
extern struct _ListNode * sysWaitForObjects;

//the following function is a internal to kernel and device drivers
LPOBJECT CreateObject(char * szName, PVOID lpData);
PVOID GetObjectData(LPOBJECT lpObj);
UINT32 DeleteObject(LPOBJECT lpObj);

//this function maintains the Signals for Objects
//void HandleObjects();

//it is the only function exposed outside
//UINT32 WaitForSingleObject(LPOBJECT lpObj);
#ifdef __cplusplus
    }
#endif
