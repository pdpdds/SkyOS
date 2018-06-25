#pragma once
#include "iostream.h"
#include "stddef.h"

namespace std
{
	class string
	{
	public:
		// Construct a blank instance. It will be length 0 and the internal cstring
		// will be a character array of length one, containing only \0.
		//
		// Examples:
		//   String a;
		//   String a();
		//   String* a = new String();
		//
		string();

		// Construct with an initial value copied from a cstring (array of
		// characters).
		//
		// This will copy the data from the source cstring. The class is done
		// with the memory at the source pointer when this constructor returns.
		//
		// Examples:
		//   String a("literal string");
		//   String* a = new String("literal string");
		//
		//   String a = "literal string";
		//   
		//   void function(String a);
		//   function("literal string");
		//
		string(const char* source);

		// Copy constructor. Create a copy of another String instance.
		//
		// This will copy the interal cstring from a source String into the newly
		// constructed String. The source string will be unchanged.
		//
		// Given:
		//   String other = "a string";
		// 
		// Examples:
		//   String a(other);
		//   String* a = new String(other);
		//
		//   String a = other;
		//
		//   void function(String a);
		//   function(other);
		//
		string(const string &source);

		// Destructor deallocates the internal dynamically allocated cstring.
		~string();

		// Get a constant pointer to the internal cstring.
		//
		// Because this gets a CONSTANT character pointer to the internal cstring,
		// you cannot change characters in the returned cstring. This means that
		// this can't be used to mess with the internal cstring of a String
		// instance.
		//
		// This method is useful when working with functions that don't know about
		// this String class and but know how to handle an old school cstring.
		const char* cstring() const;

		// Get the number of characters (not including the trailing \0) in the
		// string.
		int length() const;

		bool empty() const;

		// Get the character at a given index of the string.
		//
		// If the index is negative, then it will return the character at
		// length + i. So -1 would be the last (non-null) character in the string.
		//
		// If the length is out of bounds, the return \0.
		char get(int i) const;

		// Set the character at a given index of the string.
		//
		// Returns false if the character could not be set, either because the index
		// is out of range, or the character is a \0.
		//
		// If the index is negative, then it will set the character at
		// length + index. So -1 would be the last (non-null) character in the
		// string.
		//
		// The null terminator cannot be modified with this method because it's
		// cnosidered an out of range index.
		bool set(int i, char c);

		// Return 0 if this String is equal to the other cstring. Return less than 0
		// if this String is less than the other cstring. return greater than 0 if
		// this String is greater than the other cstring.
		//
		// This should be exactly equivalent to comparing the two cstring values
		// using std::strcmp().
		//
		// Example:
		//   String("a").compare("b") == strcmp(String("a").cstring(), "b"); // true
		//
		int compare(const char* other) const;

		// Exactly the same as the previous compare method, except that this one
		// expects another instance of String and will compare against the internal
		// cstring value of the other String.
		int compare(const string &other) const;

		// Appends the other cstring value to the end of this String.
		//
		// Note: This modifies the current string!
		//
		// Returns a reference to this String.
		string &append(const char* other);

		// Appends the other String value to the end of this string.
		//
		// Note: This modifies the current string!
		//
		// Returns a reference to this String.
		string &append(const string &other);

		// Returns a new String that has the value of this String and the other
		// cstring concatenated together.
		string concat(const char* other) const;

		// Returns a new String that has the value of this String and the other
		// String concatenated together.
		string concat(const string &other) const;

		// Returns a lower case copy of the string.
		string lower();

		// Returns an upper case copy of the string.
		string upper();

		// Returns a copy of a part of the string.
		//
		// If start is less than 0, then length - start is used as the starting
		// index.
		//
		// If length is not given or negative, then the string is copied from the
		// start index to the end.
		string substr(int start, int length = -1) const;

		// Return a copy of the string with all the white space characters removed
		// from the left side.
		//
		// Allows an optional parameter string of characters that should be
		// considered whitespace. Defaults to " \t\r\n".
		string trimleft(const char* chars = " \t\r\n") const;

		// Return a copy of the string with all the whitespace characters removed
		// from the right side.
		//
		// Allows an optional parameter string of characters that should be
		// considered whitespace. Defaults to " \t\r\n".
		string trimright(const char* chars = " \t\r\n") const;

		// Return a copy of the string with all the whitespace removed from both
		// sides.
		//
		// Allows an optional parameter string of characters that should be
		// considered whitespace. Defaults to " \t\r\n"
		string trim(const char* chars = " \t\r\n") const;

		// Read a line from a std::istream (input stream) and append it to this
		// string.
		//
		// Returns the number of characters appended.
		//
		// An optional delimiter can be given which defaults to \n. Use String::eof
		// to read everything until input ends or ctrl+d is pressed.
		//
		// Note: This will not append the trailing delimiter!
		// Note: This modifies the current string!		

		// Operator overload for the "cast" operator (const char*). Allows instances
		// of this class to be used in place of almost anything that expects a
		// constant cstring.
		//
		// Note, that it is for CONSTANT cstrings. You cannot use this to modify the
		// internal character array.
		//
		// This is probably the most important operator. Having this operator lets
		// us define later operators for const char* values, and they will
		// automatically work for other String instances too because other String
		// instances will get cast const char* using this operator this implicitly.
		//
		// Given:
		//   String other = "a string";
		//
		// Examples:
		//   strlen(other);
		//
		//   const char* a = other;               // Implicit
		//   const char* a = (const char*)other;  // Explicit
		//
		//   void function(const char* a);
		//   function(a);
		//
		operator const char* () const;

		// Allows this class to be accessed character by character like a regular
		// cstring character array. The characters are returned by value, so using
		// this operator to change a character won't work. Use the set() method
		// instead.
		char operator [] (int i) const;

		// Allow assignment from a const char*.
		string& operator = (const char* rhs);

		// Allow assignment from another instance of this class.
		string& operator = (const string &rhs);

		// Append the right hand side value to the current String.
		//
		// Note: This modifies the current string!
		string& operator += (const char* rhs);
		string& operator += (const char ch);
		string& append(const char ch);

		// Return a new String that is the left and right hand sides concatenated
		// together.
		//
		// This is a "friend" function. It's NOT actually part of the String class.
		// When you define it, you won't prefix it with "String::", and the
		// definition can't use "this".
		//
		// What being a friend DOES do, is give it access to private and protected
		// members of any String. It also lets the compiler know that there's an
		// operator + definition for the String class.
		//
		// This operator doesn't have to be defined as a friend, but defining it as
		// a friend makes it work if EITHER side of the + operator is a String,
		// instead of only when the left hand side is.
		//
		// Given:
		//   String a = "foo";
		//   String b = "bar";
		//
		// Examples:
		//   String c = a + b;
		//   String c = a + "baz";
		//   String c = "baz" + a; // Making this a "friend" makes this work.
		//
		friend string operator + (string lhs, const char* rhs);

		// Make comparison operators check character equality against a cstring or
		// String.
		bool operator == (const char* rhs) const;
		bool operator < (const char* rhs) const;
		bool operator <= (const char* rhs) const;
		bool operator > (const char* rhs) const;
		bool operator >= (const char* rhs) const;

		// Copied out of the standard library (<cstdio>/"stdio.h") for convenience.
		// Useful with getline() if you want to read in until the end of a file.
		static const char eof = EOF;

		const char* c_str() { return str; }

	private:
		// The internal cstring (character array) value of the String.
		char* str;

		// The length of the internal cstring is kept up to date so we don't have to
		// call strlen every time we want to know how long this String currently is.
		int len;

		// Return true if the c character matches any of the characters in the
		// chars cstring. The length pparameter must be set to the strlen of the
		// chars cstring.
		bool matchAny(char c, const char* chars, int length) const;
	};
}