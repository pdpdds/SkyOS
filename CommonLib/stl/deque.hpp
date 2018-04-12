namespace std
{
	template <typename T>
	deque<T>::deque(int c, bool d) : _display(d), _deque_front(c, d), _deque_rear(c, d), _front(0), _rear(0), _front_last(0), _rear_first(0)
	{
		if (display()) {
			//cout<<"In deque constructor:"<<endl;
		}
	}

	/*----------------------------------------------------------------
	Destructor
	-----------------------------------------------------------------*/
	template <typename T>
	deque<T>::~deque()
	{
		if (display()) {
			//cout<<"In deque destructor:"<<endl;
		}
		_front = 0;
		_rear = 0;
	}

	/*----------------------------------------------------------------
	[] operator
	-----------------------------------------------------------------*/
	template <typename T>
	T& deque<T>::operator[](int i)
	{
		if (i < 0) {
			_deque_rear[0];
		}
		if (i < _front) return _deque_front[(_front - 1) - i];
		else if (_front == 0) return _deque_rear[i];
		else if (i >= _front) return _deque_rear[i - _front];

		return _deque_rear[0];
	}

	/*----------------------------------------------------------------
	Get size of the deque
	-----------------------------------------------------------------*/
	template <typename T>
	int deque<T>::size() const
	{
		return _front + _rear;
	}

	/*----------------------------------------------------------------
	Check if the deque is empty
	-----------------------------------------------------------------*/
	template <typename T>
	bool deque<T>::empty() const
	{
		return (_front == 0 && _rear == 0);
	}

	/*----------------------------------------------------------------
	Push to the front of the deque
	-----------------------------------------------------------------*/
	template <typename T>
	void deque<T>::push_front(const T& b)
	{
		_deque_front[_front++] = b;
	}


	/*----------------------------------------------------------------
	Push to the back of the deque
	-----------------------------------------------------------------*/
	template <typename T>
	void deque<T>::push_back(const T& b)
	{
		_deque_rear[_rear++] = b;
	}

	/*----------------------------------------------------------------
	Get the front of the deque by alias
	-----------------------------------------------------------------*/
	template <typename T>
	T& deque<T>::front()
	{
		if (empty()) {
			assert(0);
		}
		if (_front) return (_deque_front[_front - 1]);
		else return (_deque_rear[_rear_first]);
	}

	/*----------------------------------------------------------------
	Get the back of the deque by alias
	-----------------------------------------------------------------*/
	template <typename T>
	T& deque<T>::back()
	{
		if (empty()) {
			assert(0);
		}
		if (_rear) return (_deque_rear[_rear - 1]);
		else return (_deque_front[_front_last]);
	}

	/*----------------------------------------------------------------
	Remove an element from the front of the deque
	-----------------------------------------------------------------*/
	template <typename T>
	void deque<T>::pop_front()
	{
		if (empty()) {
			assert(0);
		}
		if (_front) _front--;
		else _rear_first++;
	}

	/*----------------------------------------------------------------
	Remove an element from the back of the deque
	-----------------------------------------------------------------*/
	template <typename T>
	void deque<T>::pop_back()
	{
		if (empty()) {
			assert(0);
		}
		if (_rear) _rear--;
		else _front_last++;
	}
}