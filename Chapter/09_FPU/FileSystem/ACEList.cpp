#include "ACEList.h"
#include "string.h"
#include "SysInfo.h"
#include "SkyConsole.h"

int List_Count(LPLISTNODE  List)
{   
    int Count=0;
   // DEBUG_PRINT_OBJECT1("Called for list at %X", List);
    if ( List==NULL )
    {
      //  DEBUG_PRINT_INFO("Returns 0");
        return 0;
    }
    while(List->Next)
    {
        Count++;
        List=List->Next;
    }
    //DEBUG_PRINT_OBJECT1("Returns %d", Count+1);

    return Count+1;//1 is for the root pointer
}
/* This function add the given data to the linked list
    Returns 0 Success*/
int List_Add(LPLISTNODE * Root, LPCTSTR szKey, void * ptrData)
{
    LPLISTNODE List, NewNode, tmpList;
    List = *Root;       // Get the value of the Linked List Root
    int Len;
  //  DEBUG_PRINT_OBJECT2("Key %s data %X", szKey,ptrData);
    //CHECKING THE LIST FOR EXISTING OF KEY 
    if ( szKey[0] && List )     //ommit the key checking if key is not given or root is not exists
    {
     //   DEBUG_PRINT_OBJECT1("Searching for Key %s", szKey);
        tmpList=*Root;
        while ( tmpList )
        {
        //    DEBUG_PRINT_OBJECT1("Comparing key at node %X ",tmpList);
            if ( strcmp(tmpList->szKey,szKey) == 0 )
            {
               // DEBUG_PRINT_OBJECT1("Key already exist [%s]", szKey);
                SetLastError(ERROR_BADKEY);
                return 1;       //KEY ALREADY EXIST
            }
            tmpList=tmpList->Next;
        }
       // DEBUG_PRINT_INFO("No matching key found");
    }
    //NEW NODE CREATION AND ASSIGNING VALUES			
	NewNode = new LISTNODE;
    NewNode->Next=NULL;
    NewNode->ptrData=ptrData;
    //DEBUG_PRINT_OBJECT1("New node created at %X", NewNode );
	
    Len = strlen(szKey);
    if ( Len >= KEYSIZE )
    {
       // DEBUG_PRINT_OBJECT1("Keysize too long(%d)",Len);
      SetLastError(ERROR_BADKEY);
        return 3;               //KEYSIZE TOO LONG
    }
   // DEBUG_PRINT_OBJECT2("Copying %d length of Key(actual %d) to NewNode",Len, Len<KEYSIZE?Len:KEYSIZE-1 );
    strncpy(NewNode->szKey, szKey, Len<KEYSIZE?Len:KEYSIZE-1);
    for(;Len<KEYSIZE;Len++)
        NewNode->szKey[Len]=0;  //filling the remaining characters with 0
    if ( List==NULL )           //check the availability of first node (ROOT)
        *Root=NewNode;
    else
    {
       // DEBUG_PRINT_INFO("Finding the last node to add");
        while(List->Next)       
            List=List->Next;    
        List->Next=NewNode;
    }
   SetLastError( ERROR_SUCCESS );
    return 0;                   //SUCCESS
}
/* Delete the given node from the list
    You can refer a Node by its index in the list or its key
    If you using key pass Index as -1
**However the index is rapidly changing one as we insert, delete elements.
    
******Note Before Deletion you must delete its contents by retriving 
its pointer from the list and delete it after that call this procedure
*/
int List_Delete(_ListNode ** Root, LPCTSTR szKey, int Index)
{
    LPLISTNODE List, tmpList;
    int CurIndex=0;
    List=*Root;
    tmpList=List;
    while( List )
    {
        //this checking ensure which parameter is given
        if ( CurIndex == Index || ( List->szKey[0] && strcmp( List->szKey, szKey ) == 0 ) ) 
        {
            if ( tmpList == List ) //if we deleting root node, we have to assign a new root
                *Root=List->Next;       //new root
            else
                tmpList->Next = List->Next;     //skip the LIST node
            
            delete List;
          SetLastError( ERROR_SUCCESS );
            return 0;
        }
        tmpList=List;
        List=List->Next;
        CurIndex++;
    }
   SetLastError( ERROR_INVALID_HANDLE );
    return 1; //unable to traverse or find the node 
}
/*
this function searches for a key in the list and returns the data
*/
void * List_GetData(LPLISTNODE  List, LPCTSTR szKey,int Index)
{
    
    int CurIndex=0;
  //  DEBUG_PRINT_OBJECT2("Key[%s] Index [%d]\n",szKey,Index);

    while( List )
    {
        //this checking ensure which parameter is given
        if ( CurIndex == Index || ( szKey[0] && strcmp( List->szKey, szKey ) == 0 ) ) 
        {
        //    DEBUG_PRINT_OBJECT1("Match found returns %X", List->ptrData);
            return List->ptrData;
        }
        #ifdef DEBUG
         //   dbgprintf("%X->", List);
        #endif
        List=List->Next;
        CurIndex++;
    }
    //DEBUG_PRINT_INFO("End of list returns NULL");

    return NULL; //unable to traverse or find the node 
}
void List_Dump(LPLISTNODE  List)
{
    //DEBUG_PRINT_INFO("List : ")
    while ( List )
    {
        #ifdef DEBUG
           // dbgprintf("%X-", List);
          //  dbgprintf("[%s]%X -->", List->szKey, List->ptrData);
        #endif
        List = List->Next;
    }
}

