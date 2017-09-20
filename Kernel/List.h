#pragma once
#include "windef.h"
            
namespace Sky
{
	struct ListNode
	{
		void* ptrData;
		struct ListNode* Next;
	};

	class LinkedList
	{
	
	public:
		LinkedList();
		int Count();
		BOOL Add(void* ptrData);
		BOOL Delete(void* ptrData);
		void* Get(int index);

	private:
		ListNode* m_pNode;

	};
};

//#define FOREACH(tmpNode, ListRoot) \
    //for( tmpNode=ListRoot; tmpNode != NULL ; tmpNode = tmpNode->Next ) 
    


