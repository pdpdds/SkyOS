#pragma once
#include "Vector.h"

template<typename T>
class Stack {
private:
	Vector<T> theArray;
public:
	Stack() { theArray.clear(); }
	void push(T data) { theArray.push_back(data); }
	T pop() { T retData = theArray.back(); theArray.pop_back(); return retData; }
	size_t size() { return theArray.size(); }

};