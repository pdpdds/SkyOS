#pragma once
#include "windef.h"

namespace std
{
    class string {
    public:
    
        string();
        string(const char* s);
        string(const char* s, size_t len);
        string(const string& rhs);

        string& operator=(const string& rhs);

        ~string();

        size_t size() const;
        const char* data() const;
        const char* c_str() const;

        const char& operator[]( size_t index ) const;
        char* operator[](size_t index);

    private:
        char*  data_;
        size_t len_;
    };
}