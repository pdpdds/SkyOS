#pragma once

#ifdef _WIN64
typedef unsigned __int64 size_t;
typedef __int64          ptrdiff_t;
typedef __int64          intptr_t;
#else
typedef unsigned int     size_t;
typedef int              ptrdiff_t;
typedef int              intptr_t;
#endif

#define BUFSIZ	512
#define EOF -1

typedef int sig_atomic_t;

// minimum signed 64 bit value
#define _I64_MIN    (-9223372036854775807i64 - 1)
// maximum signed 64 bit value
#define _I64_MAX      9223372036854775807i64
// maximum unsigned 64 bit value
#define _UI64_MAX     0xffffffffffffffffui64

#define	SNG_EXP_INFNAN	255
#define	DBL_EXP_INFNAN	2047

#define	SNG_EXP_BIAS	127
#define	DBL_EXP_BIAS	1023

