#include "stl_string.h"
#include <string.h>

namespace std
{
    string::string()
        : data_(NULL), len_(0)
    {
    }

    string::string( const char* s )
    {
        len_ = strlen(s);
        if(s == NULL)
            return;
        // if s is not NULL then copy
        size_t s_len = strlen(s);
        data_ = new char[s_len + 1];
        strcpy(data_, s);
    }

    string::string( const char* s, size_t len )
    {
        if(len > len_)
            return;
        size_t i;
        data_ = new char[len + 1];
        for(i = 0; i < len; i++)
        {
            data_[i] = s[i];
        }
       // std::cout<<data_[1]<<std::endl;
        len_ = len;
    }

    string::string( const string& rhs )
    {
        len_ = rhs.len_;
        data_ = new char[len_ + 1];
        strcpy(data_, rhs.data_);
    }

    string& string::operator=( const string& rhs )
    {
        return *this;
    }

    string::~string()
    {
        delete []data_;
        len_ = 0;
    }

    size_t string::size() const
    {
        return len_;
    }

    const char* string::data() const
    {
		if (len_ <= 0)
			return NULL;

        size_t len = len_;
		char* data = new char[len];
        
        int i = 0;
        while(data_[i] != '\0')
        {
            data[i] = data_[i];
            i++;
        }
        char* tmp = data;
        return tmp;
    }

    const char* string::c_str() const
    {
        return data_;
    }

    const char& string::operator[]( size_t index ) const
    {
        return data_[index];
    }

    char* string::operator[]( size_t index )
    {
      
		if (len_ <= 0)
			return NULL;

        size_t i = 0;
        size_t len = len_;
        char* data = new char[len];
        while(i < len - index)
        {
            data[i] = data_[index+i];
            i++;
        }
        data[i] = '\0';
        char* tmp = data;
        return tmp;
    }
}
