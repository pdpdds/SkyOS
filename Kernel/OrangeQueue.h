#pragma once
#include "DoubleLinkedList.h"

typedef ListNode QueueNode;

class OrangeQueue : public DoubleLinkedList {
public:
	inline QueueNode* Enqueue(QueueNode*);
	inline QueueNode* Dequeue();
};

inline QueueNode* OrangeQueue::Enqueue(QueueNode *element)
{
	return AddToTail(element);
}

inline QueueNode* OrangeQueue::Dequeue()
{
	QueueNode *node = GetHead();
	if (node == 0)
		return 0;

	node->RemoveFromList();
	return node;
}