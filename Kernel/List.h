#pragma once
#include "windef.h"
            
namespace Orange
{
	struct ListNode
	{
		void* ptrData;
		struct ListNode* Next;
	};

	class LinkedList
	{
	
	public:
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
    


