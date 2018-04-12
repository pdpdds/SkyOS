#pragma once
#include "windef.h"
            
namespace SKY
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
		bool Add(void* ptrData);
		bool Delete(void* ptrData);
		void* Get(int index);

	private:
		ListNode* m_pNode;

	};
};

//#define FOREACH(tmpNode, ListRoot) \
    //for( tmpNode=ListRoot; tmpNode != NULL ; tmpNode = tmpNode->Next ) 
    


