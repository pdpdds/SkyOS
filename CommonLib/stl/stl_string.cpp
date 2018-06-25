#include "stl_string.h"
#include "windef.h"
#include "string.h"
#include "ctype.h"
#include "memory.h"

namespace std
{
	string::string()
	{
		this->len = 0;
		// Allocate and initialize an empty internal cstring.
		this->str = new char[1];
		this->str[0] = '\0';
	}

	string::string(const char* source)
	{
		// Set the length to the source length.
		this->len = strlen(source);
		// Allocate the internal cstring.
		this->str = new char[this->len + 1];
		memset(this->str, 0, this->len + 1);
		// Copy over the data from source
		strcpy(this->str, source);
	}

	string::string(const string &source)
	{
		// Set the length to the source length.
		this->len = strlen(source.str);

		// Allocate the internal cstring.
		this->str = new char[this->len + 1];
		memset(this->str, 0, this->len + 1);
		// Copy over the data from source.str
		strcpy(this->str, source.str);
	}

	string::~string()
	{
		delete[] this->str;
		this->str = NULL;
	}

	const char* string::cstring() const
	{
		return this->str;
	}

	int string::length() const
	{
		return this->len;
	}

	bool string::empty() const
	{
		return this->len == 0;
	}

	char string::get(int i) const
	{
		// Add the length if the index is less than zero.
		if (i < 0) {
			i += this->len;
		}

		// Return a null character if the index is out of bounds.
		if (i < 0 || i >= this->len) {
			return '\0';
		}
		else {
			return this->str[i];
		}
	}

	bool string::set(int i, char c)
	{
		// Add the length if the index is less than zero.
		if (i < 0) {
			i += this->len;
		}

		// If the index is out of bounds or the character is \0, then return
		// false.
		if (i < 0 || i >= this->len || c == '\0') {
			return false;
		}

		this->str[i] = c;

		return true;
	}

	int string::compare(const char* other) const
	{
		return strcmp(this->str, other);
	}

	int string::compare(const string &other) const
	{
		return compare(other.str);
	}

	string &string::append(const char* other)
	{
		// Keep a temporary reference to the current internal cstring.
		const char* temp = this->str;
		// We'll need the length of the current string before appending.
		int length = this->len;

		// Update the length to the new length.
		this->len = length + strlen(other);
		// Allocate a new internal cstring.
		this->str = new char[this->len + 1];
		memset(this->str, 0, this->len + 1);

		// Copy the data from the original internal cstring (temp)
		strcpy(this->str, temp);
		// Also copy the data from the other cstring, into the new internal cstring,
		// starting at the end of the original cstring's data.
		strcpy(this->str + length, other);

		// Deallocate the previous internal cstring.
		delete[] temp;

		return *this;
	}

	string &string::append(const char ch)
	{
		// Keep a temporary reference to the current internal cstring.
		const char* temp = this->str;
		// We'll need the length of the current string before appending.
		int length = this->len;

		// Update the length to the new length.
		this->len = length + 1;
		// Allocate a new internal cstring.
		this->str = new char[this->len + 1];
		memset(this->str, 0, this->len + 1);

		// Copy the data from the original internal cstring (temp)
		strcpy(this->str, temp);
		// Also copy the data from the other cstring, into the new internal cstring,
		// starting at the end of the original cstring's data.
		this->str[length] = ch;

		// Deallocate the previous internal cstring.
		delete[] temp;

		return *this;
	}

	string &string::append(const string &other)
	{
		return append(other.str);
	}

	string string::concat(const char* other) const
	{
		return string(*this).append(other);
	}

	string string::concat(const string &other) const
	{
		return string(*this).append(other);
	}

	string string::lower()
	{
		// Create a copy of this string.
		string copy(*this);

		// Access the private inner cstring and set all characters to their lower
		// equivalents.
		for (int i = copy.length() - 1; i >= 0; --i) {
			copy.str[i] = tolower(copy.str[i]);
		}

		return copy;
	}

	string string::upper()
	{
		string copy(*this);

		// Access the private inner cstring and set all characters to their upper
		// equivalents.
		for (int i = copy.length() - 1; i >= 0; --i) {
			copy.str[i] = toupper(copy.str[i]);
		}

		return copy;
	}

	string string::substr(int start, int length) const
	{
		// Normalize start.
		// 
		// If it's greater than or equal to length, return an empty string. If
		// It's negative, then use length - start.
		if (start >= this->len) {
			return string();
		}
		else if (start < 0) {
			start += this->len;
			if (start < 0) {
				start = 0;
			}
		}

		// Normalize length.
		// 
		// If it's less than 0 or greater than the current length - start, then
		// set it to the current length minus start to copy the rest of the
		// string beginning at start.
		if (length < 0 || start + length > this->len) {
			length = this->len - start;
		}

		// Create a copy of this String. We aren't going to change this
		// instance.
		string copy(*this);

		// Copy the substring data from the internal cstring to the copy's
		// internal cstring.
		strncpy(copy.str, this->str + start, length);

		// strncpy doesn't ensure a null terminator, so set it manually.
		copy.str[length] = '\0';

		// Update the length of the copy since its internal cstring has been
		// directly manipulated.
		copy.len = strlen(copy.str);

		return copy;
	}

	string string::trimleft(const char* chars) const
	{
		int start = 0;
		int chars_length = strlen(chars);

		// Find the first character that does not match a character in chars.
		while (start < this->len && matchAny(this->str[start], chars, chars_length)) {
			++start;
		}

		// Return the part of the string that does not include the leading matched
		// characters.
		return substr(start);
	}

	string string::trimright(const char* chars) const
	{
		int end = this->len - 1;
		int chars_length = strlen(chars);

		// Find the last character that does not match a character in chars.
		while (end >= 0 && matchAny(this->str[end], chars, chars_length)) {
			--end;
		}

		// Return the part of the string that does not include the trailing matched
		// characters.

		return substr(0, end + 1);
	}

	string string::trim(const char* chars) const
	{
		int end = this->len - 1;
		int start = 0;
		int chars_length = strlen(chars);

		// Find the last character that does not match a character in chars.
		while (end >= 0 && matchAny(this->str[end], chars, chars_length)) {
			--end;
		}

		// There remaining string is greater than 1 character...
		if (end > 0) {
			// ...then find the first character that does not match a character in
			// chars.
			while (start < end && matchAny(this->str[start], chars, chars_length)) {
				++start;
			}

			end -= start;
		}

		// Return the part of the string that does not include the leading and
		// trailing matched characters.
		return substr(start, end + 1);
	}

	string::operator const char* () const
	{
		return this->str;
	}

	char string::operator [] (int i) const
	{
		return get(i);
	}

	string& string::operator = (const char* rhs)
	{
		// Deallocate the current internal cstring.
		delete[] this->str;
		// Update the length to the new length.
		this->len = strlen(rhs);
		// Allocate a new interal cstring.
		this->str = new char[this->len + 1];
		memset(this->str, 0, this->len + 1);
		// Copy the data from rhs.
		strcpy(this->str, rhs);

		return *this;
	}

	string& string::operator = (const string &rhs)
	{
		// Check to see if the String is being assigned to itself. That's a
		// no-op so just return if so.
		if (this != &rhs) {
			operator=(rhs.str);
		}

		return *this;
	}

	string& string::operator += (const char* rhs)
	{
		return append(rhs);
	}

	string& string::operator += (const char ch)
	{
		return append(ch);
	}

	string operator + (string lhs, const char* rhs)
	{
		return lhs.concat(rhs);
	}
	
	string operator + (const string &lhs, const char* &rhs)
	{
		return lhs.concat(rhs);
	}

	// Comparison operators
	bool string::operator == (const char* rhs) const
	{
		return compare(rhs) == 0;
	}
	bool string::operator < (const char* rhs) const
	{
		return compare(rhs) < 0;
	}
	bool string::operator <= (const char* rhs) const
	{
		return compare(rhs) <= 0;
	}
	bool string::operator > (const char* rhs) const
	{
		return compare(rhs) > 0;
	}
	bool string::operator >= (const char* rhs) const
	{
		return compare(rhs) >= 0;
	}

	bool string::matchAny(char c, const char* chars, int length) const
	{
		for (int i = 0; i < length; ++i) {
			if (c == chars[i]) {
				return true;
			}
		}

		return false;
	}
}