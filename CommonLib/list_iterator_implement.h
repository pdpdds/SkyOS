#pragma once


namespace std
{
	template <typename ContentsType>
	ListIterator<ContentsType>::ListIterator(ListNode<ContentsType>* nodePointer)
		: myNodePointer(nodePointer) {

		if (nodePointer == NULL) {
//			throw Exception("Attempted to go outside the list", __FILE__, __LINE__);
		}
	}

	template <typename ContentsType>
	bool ListIterator<ContentsType>::operator==(ListIterator<ContentsType>& comparedIter) {
		return (*(this->myNodePointer) == *(comparedIter.myNodePointer));
	}

	template <typename ContentsType>
	bool ListIterator<ContentsType>::operator!=(ListIterator<ContentsType>& comparedIter) {
		return !(*(this) == comparedIter);
	}


	template <typename ContentsType> // postfix operator
	const ListIterator<ContentsType>& ListIterator<ContentsType>::operator++(int i) {
		ListNode<ContentsType>* temp = myNodePointer->nextNodePointer;

		/*if (temp == NULL) {
			throw Exception("Attempted to go outside the list", __FILE__, __LINE__);
		}*/

		myNodePointer = temp;
		return *this;
	}

	template <typename ContentsType> // postfix operator
	const ListIterator<ContentsType>& ListIterator<ContentsType>::operator--(int i) {
		ListNode<ContentsType>* temp = myNodePointer->previousNodePointer;

		if (temp == NULL) {
			throw Exception("Attempted to go outside the list", __FILE__, __LINE__);
		}

		myNodePointer = temp;
		return *this;
	}

	template <typename ContentsType> // prefix operator
	const ListIterator<ContentsType>& ListIterator<ContentsType>::operator++() {
		ListNode<ContentsType>* temp = myNodePointer->nextNodePointer;

	

		myNodePointer = temp;
		return *this;
	}

	template <typename ContentsType> // prefix operator
	const ListIterator<ContentsType>& ListIterator<ContentsType>::operator--() {
		ListNode<ContentsType>* temp = myNodePointer->previousNodePointer;

		if (temp == NULL) {
			throw Exception("Attempted to go outside the list", __FILE__, __LINE__);
		}

		myNodePointer = temp;
		return *this;
	}

	template <typename ContentsType>
	const ContentsType& ListIterator<ContentsType>::operator*() {
		return myNodePointer->value;
	}

	template <typename ContentsType>
	void ListIterator<ContentsType>::rewriteNodePointer(ListNode<ContentsType>* pointer) {
		if (pointer == NULL) {
			throw Exception("Attempted to get iterator pointing to NULL", __FILE__, __LINE__);
		}

		myNodePointer = pointer;
	}
}
