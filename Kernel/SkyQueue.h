#pragma once
#include "DoubleLinkedList.h"

namespace SKY
{

	typedef ListNode QueueNode;

	class SkyQueue : public DoubleLinkedList {
	public:
		inline QueueNode* Enqueue(QueueNode*);
		inline QueueNode* Dequeue();
	};

	inline QueueNode* SkyQueue::Enqueue(QueueNode *element)
	{
		return AddToTail(element);
	}

	inline QueueNode* SkyQueue::Dequeue()
	{
		QueueNode *node = GetHead();
		if (node == 0)
			return 0;

		node->RemoveFromList();
		return node;
	}
};