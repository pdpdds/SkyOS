#include "ObjMan.h"
#include "windef.h"
#include "SkyAPI.h"
#include "ACEList.h"
#include "string.h"

struct _ListNode * sysObjects = NULL;
struct _ListNode * sysWaitForObjects = NULL;

LPOBJECT CreateObject(char * szName, PVOID lpData)
{
    LPOBJECT lpObj;    
    lpObj = new OBJECT;
    
    if ( lpObj == NULL )
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    
    strncpy((char *)lpObj->szName, szName, 19);
    lpObj->szName[19] = 0;                //terminate the string -- this logic would not work if the string is smaller than 20
    lpObj->dwUses = 1;                    //first use
    //lpObj->lpTask = (LPTASK) GetTaskID();           //Current Task
    //lpObj->lpThread= GetCurrentThread();       //Current Thread
    lpObj->dwSignals = 0;                 //no signal received
    lpObj->lpIData = lpData;              //associate custom data
    
    //lpObj->lpMessageBox = lpMessageBox;   
    
    List_Add( &sysObjects, "", lpObj ); //add it to the sys objects
    
    SetLastError( ERROR_SUCCESS );
    return lpObj;
}
//why this function is created ?
PVOID GetObjectData(LPOBJECT lpObj)
{
    if ( lpObj )
    {
        SetLastError( ERROR_SUCCESS );
        return lpObj->lpIData;
    }
    SetLastError( ERROR_INVALID_PARAMETER );
    return NULL;
}
UINT32 DeleteObject(LPOBJECT lpObj)
{
    //SetLastError(ERROR_NOT_SUPPORTED);
    LPLISTNODE tmpNode;
    int i=0;
    FOREACH(tmpNode, sysObjects) 
    {
        i++;
        if ( tmpNode->ptrData == lpObj )
        {
            List_Delete( &sysObjects, "", i);
            delete lpObj;
            SetLastError( ERROR_SUCCESS );
            return 0;
        }
    }
    SetLastError( ERROR_INVALID_PARAMETER );
    return 1;
}

/*
UINT32 WaitForSingleObject(LPOBJECT lpObj)
{
    LPWAITFOROBJECT lpWObj;
    
    lpWObj = (LPWAITFOROBJECT) malloc( sizeof( WAITFOROBJECT ) );
    if ( lpWObj )
    {
        lpWObj->lpWaitForObject = lpObj;
        lpWObj->lpTask = (LPTASK)GetTaskID();
        lpWObj->lpThread = GetCurrentThread();
        
        List_Add( &sysWaitForObjects, "", lpWObj );
        
        #warning "Here synchronizaation problem may arise here so add a spinlock here"
        ((LPTHREAD)GetCurrentThread())->dwRunState = TASK_INTERRUPTIBLE;  //set Task's state to not running but interruptable

        RunScheduler();
        
        SetLastError(ERROR_SUCCESS);
        
        return 0;
    }
    
    SetLastError(ERROR_INVALID_PARAMETER);
    return 1;
}

void HandleObjects()
{
    LPWAITFOROBJECT lpWObj;
    WORD wCount;
    int i;
    wCount=List_Count(sysWaitForObjects);
    for(i=0;i<wCount;i++)
    {
        lpWObj=(LPWAITFOROBJECT)List_GetData( sysWaitForObjects, "", i);
        if ( lpWObj )
            if ( lpWObj->lpWaitForObject->dwSignals ) //signaled?
            {
                if ( !(lpWObj->lpThread->dwRunState & TASK_STOPPED) )
                {
                    lpWObj->lpThread->dwRunState |= TASK_RUNNABLE;        //ready and not running
                    //#warning The objects must be deleted immediately 
                    List_Delete( &sysWaitForObjects, "" , i );
                    free( lpWObj );                    
                }
            }
    }

}*/
