#pragma once
#include "windef.h"

#ifdef __cplusplus
	extern "C" {
#endif
            
#define KEYSIZE 30
            
struct _ListNode
{
    void * ptrData;
    char szKey[KEYSIZE];
    struct _ListNode * Next;
};

typedef struct _ListNode LISTNODE;
typedef  LISTNODE * LPLISTNODE;

int List_Count(LPLISTNODE  List);
int List_Add(LPLISTNODE * Root, LPCTSTR szKey, void * ptrData);
int List_Delete(_ListNode ** Root, LPCTSTR szKey, int Index);
void * List_GetData(LPLISTNODE  List, LPCTSTR szKey, int Index);
void List_Dump(LPLISTNODE  List);
    
#define FOREACH(tmpNode, ListRoot) \
    for( tmpNode=ListRoot; tmpNode != NULL ; tmpNode = tmpNode->Next ) 
    
#ifdef __cplusplus
	}
#endif


