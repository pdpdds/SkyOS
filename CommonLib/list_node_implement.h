#pragma once

namespace std
{
	template <typename ContentsType>
	ListNode<ContentsType>::ListNode()
		: nextNodePointer(NULL)
		, previousNodePointer(NULL) {
	}

	template <typename ContentsType>
	ListNode<ContentsType>::ListNode(const ContentsType& _value)
		: value(_value)
		, nextNodePointer(NULL)
		, previousNodePointer(NULL) {
	}

	template <typename ContentsType>
	bool ListNode<ContentsType>::operator==(ListNode& comparedNode) {
		return  (this->value == comparedNode.value)
			&& (this->previousNodePointer == comparedNode.previousNodePointer)
			&& (this->nextNodePointer == comparedNode.nextNodePointer);
	}

	template <typename ContentsType>
	bool ListNode<ContentsType>::operator!=(ListNode& comparedNode) {
		return !(*this == comparedNode);
	}
}