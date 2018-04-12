#pragma once
#include "windef.h"

namespace std
{
	struct DLNode {
	public:
		inline DLNode();
		virtual ~DLNode() {}
		inline void RemoveFromList();

		LPVOID _data;
	private:
		DLNode * fNext;
		DLNode *fPrev;
		friend class DoubleLinkedList;		
	};

	class DoubleLinkedList
	{
	public:
		inline DoubleLinkedList();
		inline DLNode* AddToTail(DLNode*);
		inline DLNode* AddToHead(DLNode*);
		inline DLNode* AddBefore(DLNode *next, DLNode *newEntry);
		inline DLNode* AddAfter(DLNode *prev, DLNode *newEntry);
		inline DLNode* Remove(DLNode*);
		inline DLNode* Remove(void*);
		inline void		 Clear();
		inline DLNode* GetHead() const;
		inline DLNode* GetTail() const;
		inline DLNode* GetNext(const DLNode*) const;
		inline DLNode* GetPrevious(const DLNode*) const;
		inline bool IsEmpty() const;
		inline int CountItems() const;

	protected:

		// The nodes are stored in a doubly-linked circular list
		// of entries, with this dummy node as the head.
		DLNode fDummyHead;

	};

	inline DLNode::DLNode()
		: fNext(0),
		fPrev(0),
		_data(0)
	{

	}

	inline void DLNode::RemoveFromList()
	{
		fPrev->fNext = fNext;
		fNext->fPrev = fPrev;
		fNext = 0;
		fPrev = 0;
	}

	inline DoubleLinkedList::DoubleLinkedList()
	{
		fDummyHead.fNext = &fDummyHead;
		fDummyHead.fPrev = &fDummyHead;
		fDummyHead._data = 0;
	}

	inline DLNode* DoubleLinkedList::AddToTail(DLNode *node)
	{
		node->fNext = &fDummyHead;
		node->fPrev = fDummyHead.fPrev;
		node->fNext->fPrev = node;
		node->fPrev->fNext = node;
		return node;
	}

	inline DLNode* DoubleLinkedList::AddToHead(DLNode *node)
	{
		node->fPrev = &fDummyHead;
		node->fNext = fDummyHead.fNext;
		node->fNext->fPrev = node;
		node->fPrev->fNext = node;
		return node;
	}

	inline DLNode* DoubleLinkedList::Remove(DLNode *node)
	{
		node->RemoveFromList();
		return node;
	}

	inline DLNode* DoubleLinkedList::Remove(void* data)
	{
		for (DLNode *node = fDummyHead.fNext; node != &fDummyHead; node = node->fNext)
		{
			if (data == node->_data)
			{
				node->RemoveFromList();
				return node;
			}
		}

		return NULL;
	}

	inline void DoubleLinkedList::Clear()
	{
		DLNode* node = fDummyHead.fNext;

		if (node == &fDummyHead)
			return;

		DLNode* tempNode = node;
		while (tempNode != nullptr)
		{
			node = tempNode;
			tempNode = node->fNext;
			delete node;
		}
	}

	inline DLNode* DoubleLinkedList::GetHead() const
	{
		if (fDummyHead.fNext == &fDummyHead)
			return 0;

		return fDummyHead.fNext;
	}

	inline DLNode* DoubleLinkedList::GetTail() const
	{
		if (fDummyHead.fPrev == &fDummyHead)
			return 0;

		return fDummyHead.fPrev;
	}

	inline DLNode* DoubleLinkedList::GetNext(const DLNode *node) const
	{
		if (node->fNext == &fDummyHead)
			return 0;

		return node->fNext;
	}

	inline DLNode* DoubleLinkedList::GetPrevious(const DLNode *node) const
	{
		if (node->fPrev == &fDummyHead)
			return 0;

		return node->fPrev;
	}

	inline DLNode* DoubleLinkedList::AddBefore(DLNode *nextEntry, DLNode *newEntry)
	{
		newEntry->fNext = nextEntry;
		newEntry->fPrev = nextEntry->fPrev;
		newEntry->fPrev->fNext = newEntry;
		newEntry->fNext->fPrev = newEntry;

		return newEntry;
	}

	inline DLNode* DoubleLinkedList::AddAfter(DLNode *previousEntry, DLNode *newEntry)
	{
		newEntry->fNext = previousEntry->fNext;
		newEntry->fPrev = previousEntry;
		newEntry->fPrev->fNext = newEntry;
		newEntry->fNext->fPrev = newEntry;

		return newEntry;
	}

	inline bool DoubleLinkedList::IsEmpty() const
	{
		return fDummyHead.fNext == &fDummyHead;
	}

	inline int DoubleLinkedList::CountItems() const
	{
		int count = 0;
		for (DLNode *node = fDummyHead.fNext; node != &fDummyHead; node = node->fNext)
			count++;

		return count;
	}
};