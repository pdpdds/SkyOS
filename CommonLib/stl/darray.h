/*----------------------------------------------------------------
Copyright (c) 2015 Author: Jagadeesh Vasudevamurthy
file: darray.h
-----------------------------------------------------------------*/

/*----------------------------------------------------------------
This file has darray class declaration
-----------------------------------------------------------------*/

/*----------------------------------------------------------------
All includes here
-----------------------------------------------------------------*/
#ifndef darray_H
#define darray_H

#include "windef.h"

/*----------------------------------------------------------------
Declaration of darray class
-----------------------------------------------------------------*/
namespace std
{
	template <typename T>
	class darray {
	public:
		explicit darray(int c = 50, bool d = false);
		explicit darray(bool);
		explicit darray(bool d, int c) = delete;
		darray(const T f[], int c, bool display = false); //For constant object.
		~darray();
		darray(const darray<T>& s);
		darray<T>& operator=(const darray<T>& rhs);
		T& operator[](int i); //For non constant objects
		const T& operator[](int i) const; //for constant objects
		bool display()const { return _display; }
		void set_display(bool x) {
			_display = x;
		}
		int size() const {
			return _size;
		}
	private:
		T * _element;
		int _capacity;
		bool _display;
		int _size; //largest accessed + 1
		void _copy(const darray<T>& s);
		void _release() { delete[] _element; }
		void _grow(int i);
	};
};
#include "darray.hpp"

#endif

//EOF

