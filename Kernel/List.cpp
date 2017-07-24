#include "List.h"
#include "string.h"
#include "sprintf.h"

namespace Orange
{

		
	int LinkedList::Count()
	{
		int count = 0;
		if (m_pNode == NULL)
		{
			return 0;
		}

		ListNode* node = m_pNode;
		while (node->Next)
		{
			count++;
			node = node->Next;
		}		

		return count + 1;
	}
	
//중복해서 같은 값을 집어 넣을수 있으므로 주의한다.
	BOOL LinkedList::Add(void * ptrData)
	{
		ListNode* pList = NULL;
		ListNode* newNode = NULL;
				
		pList = m_pNode;		
				
		newNode = new ListNode();
		newNode->Next = NULL;
		newNode->ptrData = ptrData;
				
		if (pList == NULL)
			m_pNode = newNode;
		else
		{			
			while (pList->Next)
				pList = pList->Next;
			pList->Next = newNode;
		}
		
		return true;
	}
	
	BOOL LinkedList::Delete(void* ptrData)
	{
		ListNode* pList;
		ListNode* tmpList;		
		pList = m_pNode;
		tmpList = pList;
		while (pList)
		{
		
			if(pList->ptrData == ptrData)
			{
				if (m_pNode == pList)
				{
					delete m_pNode;
					m_pNode = NULL;
				}
				else
				{
					tmpList->Next = pList->Next;
					delete pList;
				}

				return TRUE;
			}	

			tmpList = pList;
			pList = pList->Next;			
		}		
		return FALSE; 
	}
	
	void * LinkedList::Get(int index)
	{
		int curIndex = 0;
		ListNode* pList;		
		pList = m_pNode;

		while (pList)
		{			
			if (curIndex == index )
			{				
				return pList->ptrData;
			}

			pList = pList->Next;
			curIndex++;
		}		

		return NULL;
	}	
}