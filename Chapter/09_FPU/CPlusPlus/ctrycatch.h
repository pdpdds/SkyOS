/*
 This file is a part of CTryCatch library ( https://github.com/Jamesits/CTryCatch ).
 
 The MIT License (MIT)
 
 Copyright (c) 2016 James Swineson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#pragma once
#ifndef ctrycatch_h
#define ctrycatch_h

#include <setjmp.h>

// Some macro magic
#define CTRYCATCH_CAT(a, ...) CTRYCATCH_PRIMITIVE_CAT(a, __VA_ARGS__)
#define CTRYCATCH_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define CTRYCATCH_NAME(X) CTRYCATCH_CAT(__ctrycatch_, X)

// New block arguments
#define try \
    if(!(CTRYCATCH_NAME(exception_type) = setjmp(CTRYCATCH_NAME(exception_env))))

#define catch(X) \
    else if((X +0) == 0 || CTRYCATCH_NAME(exception_type) == (X +0))

#define finally

#define throw(X,...) \
    CTRYCATCH_NAME(exception_message) = (__VA_ARGS__  +0), longjmp(CTRYCATCH_NAME(exception_env), (X))

// Exception type
typedef int CTRYCATCH_NAME(exception_types);

// Global variables to store exception details
extern jmp_buf CTRYCATCH_NAME(exception_env);
extern CTRYCATCH_NAME(exception_types) CTRYCATCH_NAME(exception_type);
extern char *CTRYCATCH_NAME(exception_message);

// Helper functions
#define __ctrycatch_exception_message_exists (bool)CTRYCATCH_NAME(exception_message)

// Exception types
#ifdef Exception
#undef Exception
#endif

enum exception_type {
    Exception, // Caution: 0 **IS** defined as "no error" to make it work. DO NOT modify this line. 
#include "ctrycatch_exception_types.conf"
};

#define Exception 0

#endif /* ctrycatch_h */
