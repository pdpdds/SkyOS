// Predefined symbols and macros -*- C++ -*-

// Copyright (C) 1997-2017 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file bits/c++config.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{iosfwd}
 */
//#include "stddef.h"
#ifndef _GLIBCXX_CXX_CONFIG_H
#define _GLIBCXX_CXX_CONFIG_H 1

// The major release number for the GCC release the C++ library belongs to.
#define _GLIBCXX_RELEASE

#ifndef _GLIBCXX_PSEUDO_VISIBILITY
# define _GLIBCXX_PSEUDO_VISIBILITY(V)
#endif

# define _GLIBCXX_VISIBILITY(V) _GLIBCXX_PSEUDO_VISIBILITY(V)

# define _GLIBCXX_BEGIN_NAMESPACE_VERSION namespace __8 {
# define _GLIBCXX_END_NAMESPACE_VERSION }

// The datestamp of the C++ library in compressed ISO date format.
#define __GLIBCXX__

#ifndef _GLIBCXX_CONSTEXPR
# if __cplusplus >= 201103L
#  define _GLIBCXX_CONSTEXPR constexpr
#  define _GLIBCXX_USE_CONSTEXPR constexpr
# else
#  define _GLIBCXX_CONSTEXPR
#  define _GLIBCXX_USE_CONSTEXPR const
# endif
#endif
#endif