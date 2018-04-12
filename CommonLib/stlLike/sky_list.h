#pragma once

namespace std
{
	template <typename ContentsType> class ListNode;
	template <typename ContentsType> class ListIterator;
	template <typename ContentsType> class List;
	/**
	 * Node class for doubly linked list
	 */
	template <typename ContentsType>
	class ListNode {
		friend class List<ContentsType>;
	public:
		ListNode* nextNodePointer;
		ListNode* previousNodePointer;
		ContentsType value;

		bool operator==(ListNode&);
		bool operator!=(ListNode&);
	private:
		/**
		* Constructs an empty element of the list, points to NULL by default
		*/
		ListNode();

		/**
		 * Constructs an element of the list, points to NULL by default
		 */
		ListNode(const ContentsType& _value);
	};

	/**
	 * List iterator class for doubly linked list, has basic arithmetic
	 * operators overloaded for easy list usage
	 */
	template <typename ContentsType>
	class ListIterator {
		friend class List<ContentsType>;
	public:
		bool operator==(ListIterator&);
		bool operator!=(ListIterator&);
		const ListIterator& operator++(int);
		const ListIterator& operator++();
		const ListIterator& operator--(int);
		const ListIterator& operator--();
		const ContentsType& operator*();

	private:
		typedef ListNode<ContentsType> Node;

		Node* myNodePointer;

		/**
		 * Constructs an iterator of the list
		 * @param nodePointer Iterator points to the associated node
		 */
		explicit ListIterator(ListNode<ContentsType>* nodePointer);

		void rewriteNodePointer(Node*);
	};

	/**
	 * Doubly linked list class
	 */
	template <typename ContentsType>
	class List {
	public:
		typedef ListIterator<ContentsType> Iterator;
		typedef ListNode<ContentsType> Node;

		/**
		 * Constructs an empty list
		 */
		List();

		/**
		* Constructs a list with specified number of empty nodes
		* @param initSize Number of empty nodes in the list
		*/
		explicit List(uint initSize);

		/**
		 * Constructs a list
		 * @param elementsNumber Number of list nodes
		 * @param _value Value saved in all created list nodes
		 */
		explicit List(uint elementsNumber, const ContentsType& _value);

		virtual ~List();


		Iterator begin() const;
		Iterator end() const;

		ContentsType& front();
		ContentsType& back();
		const ContentsType& front() const;
		const ContentsType& back() const;

		/**
		 * Adds a node to the front of the list
		 * @param _value Value saved in the new list node
		 */
		void push_front(const ContentsType& _value);
		/**
		 * Adds an empty node to the front of the list
		 */
		void push_front();

		/**
		 * Adds a node to the back of the list
		 * @param _value Value saved in the new list node
		 */
		void push_back(const ContentsType& _value);
		/**
		 * Adds an empty node to the back of the list
		 */
		void push_back();

		void pop_front();
		void pop_back();

		/**
		 * Concatenates calling list and the list given as an arguement
		 * @param listToAdd Elements of this list will be placed to the end of
		 * the calling list, will be left empty afterwards (not deleted)
		 */
		void concat(List& listToAdd);

		/**
		 * Adds elements of the list given as an arguement to the calling list at the iterator position
		 * @param iter adds elements after this iterator position
		 * @param listToAdd elements of this list will be added to the calling list,
		 * will be left empty afterwards (not deleted)
		 */
		void splice(const Iterator& iter, List& listToAdd);

		bool empty() const;

		/**
		 * Inserts the node before the node corresponding to the given iterator
		 * @param Iter insert the new node before the node corrsponding to this iterator
		 * @param _value New node will store this value
		 * @return The iterator for the new node
		 */
		Iterator insert(const Iterator& iter, const ContentsType& _value);

		/**
		 * Deletes the node corresponding to the given iterator
		 * @param iter Iterator of the element to delete
		 * @return The iterator for the next node or for the end of the list if the last element was deleted
		 */
		Iterator erase(const Iterator& iter);

		/**
		 * Deletes all the elements of the list
		 */
		void clear();

		unsigned long int size() const;

	private:
		Node* getFirstNodePointer();
		Node* getLastNodePointer();

		void rewriteFirstNodePointer(Node* newNode);
		void rewriteLastNodePointer(Node* newNode);

		void connectNodes(Node* myNode, Node* newNode);

		void deleteNode(Node*);

		void insertNode(ListNode<ContentsType>* newNode,
			bool insertOrder,
			ListNode<ContentsType>* existingNode);
		void insertNodeToNotEmptyList(ListNode<ContentsType>* newNode,
			bool insertOrder,
			ListNode<ContentsType>* existingNode);
		void insertOneNodeBeforeAnother(ListNode<ContentsType>* newNode,
			ListNode<ContentsType>* subsequentNode);

		Node* firstNodePointer;
		Node* lastNodePointer;

		ListNode<ContentsType>* END_OF_LIST = NULL;
		ListNode<ContentsType>* START_OF_LIST = NULL;
		ListNode<ContentsType>* LIST_BORDER = NULL;

		static const bool INSERT_BEFORE = true;
		static const bool INSERT_AFTER = false;
	};
}
#include "list_implement.h"
