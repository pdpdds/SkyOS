#pragma once
#include "DoubleLinkedList.h"

namespace std
{

	typedef DLNode QueueNode;

	class queue : public DoubleLinkedList {
	public:
		inline QueueNode* Enqueue(QueueNode*);
		inline QueueNode* Dequeue();
	};

	inline QueueNode* queue::Enqueue(QueueNode *element)
	{
		return AddToTail(element);
	}

	inline QueueNode* queue::Dequeue()
	{
		QueueNode *node = GetHead();
		if (node == 0)
			return 0;

		node->RemoveFromList();
		return node;
	}
};