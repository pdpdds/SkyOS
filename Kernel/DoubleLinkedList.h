#pragma once

struct ListNode {
public:
	inline ListNode();
	virtual ~ListNode() {}
	inline void RemoveFromList();

	LPVOID _data;
private:
	ListNode *fNext;
	ListNode *fPrev;
	friend class DoubleLinkedList;
	friend class OrangeQueue;
};

class DoubleLinkedList {
public:
	inline DoubleLinkedList();
	inline ListNode* AddToTail(ListNode*);
	inline ListNode* AddToHead(ListNode*);
	inline ListNode* AddBefore(ListNode *next, ListNode *newEntry);
	inline ListNode* AddAfter(ListNode *prev, ListNode *newEntry);
	inline ListNode* Remove(ListNode*);
	inline ListNode* Remove(void*);
	inline ListNode* GetHead() const;
	inline ListNode* GetTail() const;
	inline ListNode* GetNext(const ListNode*) const;
	inline ListNode* GetPrevious(const ListNode*) const;
	inline bool IsEmpty() const;
	inline int CountItems() const;

protected:

	// The nodes are stored in a doubly-linked circular list
	// of entries, with this dummy node as the head.
	ListNode fDummyHead;

};

inline ListNode::ListNode()
	: fNext(0),
	fPrev(0)
{

}

inline void ListNode::RemoveFromList()
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
}

inline ListNode* DoubleLinkedList::AddToTail(ListNode *node)
{	
	node->fNext = &fDummyHead;
	node->fPrev = fDummyHead.fPrev;
	node->fNext->fPrev = node;
	node->fPrev->fNext = node;
	return node;
}

inline ListNode* DoubleLinkedList::AddToHead(ListNode *node)
{	
	node->fPrev = &fDummyHead;
	node->fNext = fDummyHead.fNext;
	node->fNext->fPrev = node;
	node->fPrev->fNext = node;
	return node;
}

inline ListNode* DoubleLinkedList::Remove(ListNode *node)
{
	node->RemoveFromList();
	return node;
}

inline ListNode* DoubleLinkedList::Remove(void* data)
{
	for (ListNode *node = fDummyHead.fNext; node != &fDummyHead; node = node->fNext)
	{
		if (data == node->_data)
		{
			node->RemoveFromList();
			return node;
		}
	}

	return NULL;
}

inline ListNode* DoubleLinkedList::GetHead() const
{
	if (fDummyHead.fNext == &fDummyHead)
		return 0;

	return fDummyHead.fNext;
}

inline ListNode* DoubleLinkedList::GetTail() const
{
	if (fDummyHead.fPrev == &fDummyHead)
		return 0;

	return fDummyHead.fPrev;
}

inline ListNode* DoubleLinkedList::GetNext(const ListNode *node) const
{
	if (node->fNext == &fDummyHead)
		return 0;

	return node->fNext;
}

inline ListNode* DoubleLinkedList::GetPrevious(const ListNode *node) const
{
	if (node->fPrev == &fDummyHead)
		return 0;

	return node->fPrev;
}

inline ListNode* DoubleLinkedList::AddBefore(ListNode *nextEntry, ListNode *newEntry)
{	
	newEntry->fNext = nextEntry;
	newEntry->fPrev = nextEntry->fPrev;
	newEntry->fPrev->fNext = newEntry;
	newEntry->fNext->fPrev = newEntry;

	return newEntry;
}

inline ListNode* DoubleLinkedList::AddAfter(ListNode *previousEntry, ListNode *newEntry)
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
	for (ListNode *node = fDummyHead.fNext; node != &fDummyHead; node = node->fNext)
		count++;

	return count;
}