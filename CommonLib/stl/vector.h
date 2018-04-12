#pragma once

namespace std
{
	template <class T>
	class  vector {
	public:

		typedef T* Iterator;

		vector();
		vector(unsigned int size);
		vector(unsigned int size, const T & initial);
		vector(const vector<T>& v);
		~vector();

		unsigned int capacity() const;
		unsigned int size() const;
		bool empty() const;
		Iterator begin();
		Iterator end();
		T& front();
		T& back();
		void push_back(const T& value);
		void pop_back();

		void reserve(unsigned int capacity);
		void resize(unsigned int size);

		T & operator[](unsigned int index);
		vector<T> & operator = (const vector<T> &);
		void clear();
	private:
		unsigned int _size;
		unsigned int _capacity;
		unsigned int Log;
		T* buffer;
	};

	template<class T>
	vector<T>::vector() {
		_capacity = 0;
		_size = 0;
		buffer = 0;
		Log = 0;
	}

	template<class T>
	vector<T>::vector(const vector<T> & v) {
		_size = v._size;
		Log = v.Log;
		_capacity = v._capacity;
		buffer = new T[_size];
		for (unsigned int i = 0; i < _size; i++)
			buffer[i] = v.buffer[i];
	}

	template<class T>
	vector<T>::vector(unsigned int size) {
		_size = size;
		Log = ceil(log((double)size) / log(2.0));
		_capacity = 1 << Log;
		buffer = new T[_capacity];
	}

	template <class T>
	bool vector<T>::empty() const {
		return _size == 0;
	}

	template<class T>
	vector<T>::vector(unsigned int size, const T& initial) {
		_size = size;
		Log = ceil(log((double)size) / log(2.0));
		_capacity = 1 << Log;
		buffer = new T[_capacity];
		for (unsigned int i = 0; i < size; i++)
			buffer[i] = initial;
	}

	template<class T>
	vector<T>& vector<T>::operator = (const vector<T> & v) {
		delete[] buffer;
		_size = v._size;
		Log = v.Log;
		_capacity = v._capacity;
		buffer = new T[_capacity];
		for (unsigned int i = 0; i < _size; i++)
			buffer[i] = v.buffer[i];
		return *this;
	}

	template<class T>
	typename vector<T>::Iterator vector<T>::begin() {
		return buffer;
	}

	template<class T>
	typename vector<T>::Iterator vector<T>::end() {
		return buffer + size();
	}

	template<class T>
	T& vector<T>::front() {
		return buffer[0];
	}

	template<class T>
	T& vector<T>::back() {
		return buffer[_size - 1];
	}

	template<class T>
	void vector<T>::push_back(const T & v) {
		/*
		Incidentally, one common way of regrowing an array is to double the size as needed.
		This is so that if you are inserting n items at most only O(log n) regrowths are performed
		and at most O(n) space is wasted.
		*/
		if (_size >= _capacity) {
			reserve(1 << Log);
			Log++;
		}
		buffer[_size++] = v;
	}

	template<class T>
	void vector<T>::pop_back() {
		_size--;
	}

	template<class T>
	void vector<T>::reserve(unsigned int capacity) {
		T * newBuffer = new T[capacity];

		for (unsigned int i = 0; i < _size; i++)
			newBuffer[i] = buffer[i];

		_capacity = capacity;
		delete[] buffer;
		buffer = newBuffer;
	}

	template<class T>
	unsigned int vector<T>::size() const {
		return _size;
	}

	template<class T>
	void vector<T>::resize(unsigned int size) {
		Log = ceil(log((double)size) / log(2.0));
		reserve(1 << Log);
		_size = size;
	}

	template<class T>
	T& vector<T>::operator[](unsigned int index) {
		return buffer[index];
	}

	template<class T>
	unsigned int vector<T>::capacity()const {
		return _capacity;
	}

	template<class T>
	vector<T>::~vector() {
		delete[] buffer;
	}

	template <class T>
	void vector<T>::clear() {
		_capacity = 0;
		_size = 0;
		buffer = 0;
		Log = 0;
	}
}