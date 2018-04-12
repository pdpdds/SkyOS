#pragma once
#include "vector.h"

namespace std
{
	template<typename T>
	class stack {
	private:
		vector<T> theArray;
	public:
		stack() { theArray.clear(); }
		void push(T data) { theArray.push_back(data); }
		T pop() { T retData = theArray.back(); theArray.pop_back(); return retData; }
		size_t size() { return theArray.size(); }
	};
}