#pragma once
#include "darray.h"

namespace std
{
	/*----------------------------------------------------------------
	All forward declaration
	-----------------------------------------------------------------*/
	template <typename T>
	class deque;

	template <typename T>
	class deque_iterator;

	/*--------------------------------------------------------
	typename deque iterator
	----------------------------------------------------------*/
	template <typename T>
	class deque_iterator {
	public:
		deque_iterator(T* front_queue = 0, int front = 0, T *rear_queue = 0, int rear = 0, T* current = 0) : _current(current), _fq(front_queue), _front(front), _rq(rear_queue), _rear(rear) {}
		~deque_iterator() {}

		// *(itt)
		T& operator*() const {
			return *(_current);
		}

		//++itt
		deque_iterator& operator++() {
			if (_front > 0)
			{
				--_current;
				_front--;
				return *this;
			}
			else if (_front == 0)
			{
				_current = _rq;
				_front--;
				return *this;
			}
			else
			{
				++_current;
				//_rear++;
				return *this;
			}
		}

		//--itt
		deque_iterator& operator--() {
			if (_rear > 0)
			{
				--_current;
				_rear--;
				return *this;
			}
			else if (_rear == 0)
			{
				_current = _fq;
				_rear--;
				return *this;
			}
			else
			{
				++_current;
				//_front++;
				return *this;
			}
		}

		//if (itt != x.end())
		bool operator!=(const deque_iterator& rhs) {
			return (_current != rhs._current);
		}

	private:
		T* _current;
		T* _fq;;
		int _front;
		T* _rq;
		int _rear;
	};

	/*--------------------------------------------------------
	class deque
	----------------------------------------------------------*/
	template <typename T>
	class deque {
	public:
		/* WRITE ALL PUBLIC FUNCTION HERE */
		/* CANNOT HAVE ANY PUBLIC DATA HERE */
		typedef deque_iterator<T> iterator;
		explicit deque(int capacity = 50, bool d = false);
		~deque();
		T& operator[](int i);

		iterator begin() { return iterator(&_deque_front[_front - 1], _front - 1, &_deque_rear[_rear_first], _rear_first, &_deque_front[_front - 1]); }
		iterator end() { return iterator(&_deque_front[_front_last], _front_last, &_deque_rear[_rear], _rear, &_deque_rear[_rear]); }
		int size() const;
		bool empty() const;
		void push_front(const T& b);  //add element to the front
		void push_back(const T& b);   //add element to the back
		T& front();                   //Get front by alias
		T& back();                    //Get back by alias
		void pop_front();             //Remove element at the front of the queue
		void pop_back();              //Remove element at the end of the queue

		bool display() const { return _display; }
		void set_display(bool x) {
			darray<T>::set_display(x);
			_display = x;
		}

	private:
		bool _display;
		darray<T> _deque_front;
		darray<T> _deque_rear;
		int _front;
		int _front_last;
		int _rear;
		int _rear_first;

		/* CAN HAVE ANY PRIVATE FUNCION */
		void _put(int pos, const T& a);
	};
}

#include "deque.hpp"


