
#include "list_node_implement.h"
#include "list_iterator_implement.h"

namespace std
{

	template <typename ContentsType>
	list<ContentsType>::list()
		: firstNodePointer(NULL)
		, lastNodePointer(NULL) {
	}

	template <typename ContentsType>
	list<ContentsType>::list(uint initSize)
		: firstNodePointer(NULL)
		, lastNodePointer(NULL) {

		for (int element = 1; element <= initSize; ++element) {
			push_front();
		}
	}

	template <typename ContentsType>
	list<ContentsType>::list(uint elementsNumber, const ContentsType& _value)
		: firstNodePointer(NULL)
		, lastNodePointer(NULL) {

		for (int element = 1; element <= elementsNumber; ++element) {
			push_front(_value);
		}
	}

	template <typename ContentsType>
	list<ContentsType>::~list() {
		clear();
	}

	template <typename ContentsType>
	listIterator<ContentsType> list<ContentsType>::begin() const {
		return Iterator(firstNodePointer);
	}

	template <typename ContentsType>
	listIterator<ContentsType> list<ContentsType>::end() const {
		return Iterator(lastNodePointer);
	}

	template <typename ContentsType>
	ContentsType& list<ContentsType>::front() {
		if (empty()) {
			throw Exception("Attempted to access non-exsistent node", __FILE__, __LINE__);
		}

		return firstNodePointer->value;
	}

	template <typename ContentsType>
	const ContentsType& list<ContentsType>::front() const {
		return front();
	}

	template <typename ContentsType>
	ContentsType& list<ContentsType>::back() {
		if (empty()) {
			throw Exception("Attempted to access non-exsistent node", __FILE__, __LINE__);
		}

		return lastNodePointer->value;
	}

	template <typename ContentsType>
	const ContentsType& list<ContentsType>::back() const {
		return back();
	}

	template <typename ContentsType>
	ListNode<ContentsType>* list<ContentsType>::getFirstNodePointer() {
		return firstNodePointer;
	}

	template <typename ContentsType>
	ListNode<ContentsType>* list<ContentsType>::getLastNodePointer() {
		return lastNodePointer;
	}

	template <typename ContentsType>
	void list<ContentsType>::rewriteFirstNodePointer(ListNode<ContentsType>* newNode) {
		firstNodePointer = newNode;
	}

	template <typename ContentsType>
	void list<ContentsType>::rewriteLastNodePointer(ListNode<ContentsType>* newNode) {
		lastNodePointer = newNode;
	}

	template <typename ContentsType>
	void list<ContentsType>::push_front(const ContentsType& _value) {
		insertNode(new Node(_value), INSERT_BEFORE, firstNodePointer);
	}

	template <typename ContentsType>
	void list<ContentsType>::push_front() {
		insertNode(new Node(), INSERT_BEFORE, firstNodePointer);
	}

	template <typename ContentsType>
	void list<ContentsType>::push_back(const ContentsType& _value) {
		insertNode(new Node(_value), INSERT_BEFORE, END_OF_LIST);
	}

	template <typename ContentsType>
	void list<ContentsType>::push_back() {
		insertNode(new Node(), INSERT_BEFORE, END_OF_LIST);
	}

	template <typename ContentsType>
	void list<ContentsType>::insertNode(ListNode<ContentsType>* newNode,
		bool insertOrder,
		ListNode<ContentsType>* existingNode) {
		if (empty()) {
			rewriteFirstNodePointer(newNode);
			rewriteLastNodePointer(newNode);
		}
		else {
			insertNodeToNotEmptylist(newNode, insertOrder, existingNode);
		}
	}

	template <typename ContentsType>
	void list<ContentsType>::insertNodeToNotEmptylist(ListNode<ContentsType>* newNode,
		bool insertOrder,
		ListNode<ContentsType>* existingNode) {
		ListNode<ContentsType>* subsequentNode = 0;

		switch (insertOrder) {
		case INSERT_BEFORE:
			subsequentNode = existingNode;
			break;
		case INSERT_AFTER:
			if (existingNode == END_OF_LIST) {
				subsequentNode = END_OF_LIST;
			}
			else {
				subsequentNode = existingNode->nextNodePointer;
			}
			break;
		}

		insertOneNodeBeforeAnother(newNode, subsequentNode);
	}

	template <typename ContentsType>
	void list<ContentsType>::insertOneNodeBeforeAnother(ListNode<ContentsType>* newNode,
		ListNode<ContentsType>* subsequentNode) {
		ListNode<ContentsType>* oldPreviousNode;

		if (subsequentNode == END_OF_LIST) {
			oldPreviousNode = lastNodePointer;
			rewriteLastNodePointer(newNode);
		}
		else {
			oldPreviousNode = subsequentNode->previousNodePointer;
			subsequentNode->previousNodePointer = newNode;
		}

		if (oldPreviousNode == START_OF_LIST) {
			rewriteFirstNodePointer(newNode);
		}
		else {
			oldPreviousNode->nextNodePointer = newNode;
		}

		newNode->nextNodePointer = subsequentNode;
		newNode->previousNodePointer = oldPreviousNode;
	}

	template <typename ContentsType>
	void list<ContentsType>::pop_front() {
		if (empty()) {
			throw Exception("Attempted to access non-exsistent node", __FILE__, __LINE__);
		}

		deleteNode(firstNodePointer);
	}

	template <typename ContentsType>
	void list<ContentsType>::pop_back() {
		if (empty()) {
			//throw Exception("Attempted to access non-exsistent node", __FILE__, __LINE__);
			return;
		}

		deleteNode(lastNodePointer);
	}

	template <typename ContentsType>
	void list<ContentsType>::deleteNode(ListNode<ContentsType>* nodeToDelete) {
		ListNode<ContentsType>* nextNodePointer = nodeToDelete->nextNodePointer;
		ListNode<ContentsType>* previousNodePointer = nodeToDelete->previousNodePointer;

		delete nodeToDelete;

		if (previousNodePointer != START_OF_LIST) {
			previousNodePointer->nextNodePointer = nextNodePointer;
		}
		else {
			rewriteFirstNodePointer(nextNodePointer);
		}

		if (nextNodePointer != END_OF_LIST) {
			nextNodePointer->previousNodePointer = previousNodePointer;
		}
		else {
			rewriteLastNodePointer(previousNodePointer);
		}
	}

	template <typename ContentsType>
	void list<ContentsType>::concat(list<ContentsType>& listToAdd) {
		if (!listToAdd.empty()) {
			ListNode<ContentsType>* listToAddFirstNode = listToAdd.getFirstNodePointer();
			ListNode<ContentsType>* listToAddLastNode = listToAdd.getLastNodePointer();
			ListNode<ContentsType>* thislistLastNode = this->getLastNodePointer();

			if (thislistLastNode == LIST_BORDER) {
				this->rewriteFirstNodePointer(listToAddFirstNode);
			}
			else {
				thislistLastNode->nextNodePointer = listToAddFirstNode;
			}

			this->rewriteLastNodePointer(listToAddLastNode);
			listToAddFirstNode->previousNodePointer = thislistLastNode;
			listToAdd.rewriteFirstNodePointer(NULL);
			listToAdd.rewriteLastNodePointer(NULL);
		}
	}

	template <typename ContentsType>
	void list<ContentsType>::splice(const listIterator<ContentsType>& iter, list<ContentsType>& listToAdd) {
		if (!listToAdd.empty()) {
			ListNode<ContentsType>* listToAddFirstNode = listToAdd.getFirstNodePointer();
			ListNode<ContentsType>* listToAddLastNode = listToAdd.getLastNodePointer();

			ListNode<ContentsType>* iterNodePointer = iter.myNodePointer;
			ListNode<ContentsType>* afterIterNodePointer = iterNodePointer->nextNodePointer;

			listToAddFirstNode->previousNodePointer = iterNodePointer;
			listToAddLastNode->nextNodePointer = afterIterNodePointer;
			iterNodePointer->nextNodePointer = listToAddFirstNode;

			if (afterIterNodePointer == LIST_BORDER) {
				rewriteLastNodePointer(listToAddLastNode);
			}
			else {
				afterIterNodePointer->previousNodePointer = listToAddLastNode;
			}

			listToAdd.rewriteFirstNodePointer(NULL);
			listToAdd.rewriteLastNodePointer(NULL);
		}
	}

	template <typename ContentsType>
	bool list<ContentsType>::empty() const {
		return (firstNodePointer == NULL) && (lastNodePointer == NULL);
	}

	template <typename ContentsType>
	listIterator<ContentsType> list<ContentsType>::insert(const listIterator<ContentsType>& iter,
		const ContentsType& _value) {
		ListNode<ContentsType>* newNode = new Node(_value);
		insertNode(newNode, INSERT_BEFORE, iter.myNodePointer);
		return Iterator(newNode);
	}

	template <typename ContentsType>
	listIterator<ContentsType> list<ContentsType>::erase(const listIterator<ContentsType>& iter) {
		ListNode<ContentsType>* nodeToPointForReturnedIter = iter.myNodePointer->nextNodePointer;

		if (iter == end()) {
			nodeToPointForReturnedIter = lastNodePointer->previousNodePointer;
		}

		deleteNode(iter.myNodePointer);
		iter.myNodePointer = NULL;

		return Iterator(nodeToPointForReturnedIter);
	}

	template <typename ContentsType>
	void list<ContentsType>::clear() {
		while (!empty()) {
			deleteNode(firstNodePointer);
		}
	}

	template <typename ContentsType>
	unsigned long int list<ContentsType>::size() const {
		ListNode<ContentsType>* nextNodePointer = firstNodePointer;
		unsigned long int counter = 0;

		while (nextNodePointer != NULL) {
			++counter;
			nextNodePointer = nextNodePointer->nextNodePointer;
		}

		return counter;
	}
}