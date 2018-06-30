#pragma once
#include "limits.h"

 #define DLLEXPORT __declspec(dllexport)
 #define DLLIMPORT __declspec(dllimport)

#define SKYAPI

#define KERNELDLL 
/*
  #define CHAR_BIT            8

  #define CHAR_MAX            127
  #define CHAR_MIN            (-128)

  #define BYTE_MAX           255
  #define BYTE_MIN            0

  #define INT_MAX             0x7FFF
  #define INT_MIN             ((int)0x8000)
  #define UINT_MAX            0xFFFFU

  #define LONG_MAX            0x7FFFFFFFL
  #define LONG_MIN            ((long)0x80000000L)
  #define ULONG_MAX           0xFFFFFFFFUL*/

#define MAXPATH	256
#define MAX_PROCESS_NAME	256


  /* 8 Bit data types*/
  typedef unsigned char BYTE;

  /* 16 bit data types */
  typedef short INT16;
  typedef unsigned short UINT16;
  typedef unsigned short WORD;
  typedef unsigned short USHORT;
  typedef long INT32;
  typedef unsigned long UINT32;
  typedef unsigned int UINT;
  typedef unsigned long DWORD;
  typedef DWORD *LPDWORD;
  typedef DWORD *PDWORD;
  typedef int              ptrdiff_t;
typedef unsigned long  ulong;			//32 bit unsigined integer
typedef unsigned int   uint;			//32 bit unsigined integer
typedef unsigned short ushort;			//16 bit integer
typedef unsigned char  uchar;			//8 bit integer

typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;

typedef long int __time_t;
typedef __time_t time_t;

  #ifndef _SIZE_T_DEFINED
    typedef unsigned int size_t;
	typedef unsigned int SIZE_T;
    #define _SIZE_T_DEFINED
  #endif

  #define CONST const

  #ifndef VOID
      #define VOID void
  #endif
  typedef char CHAR;
  typedef unsigned char UCHAR;
  typedef unsigned char BYTE;
  typedef UCHAR				*PUCHAR;
  typedef short SHORT;
  typedef long LONG;
  typedef void *PVOID;
  typedef PVOID HANDLE;
  typedef HANDLE HWND;
  
  typedef unsigned long		ULONG;
  typedef long long			QWORD;

  typedef long               ISIZE;
  typedef unsigned long      LSIZE;
  typedef unsigned long      USIZE;
  #define CALLBACK
  typedef int WPARAM;
  typedef int LPARAM;
	  
  typedef HANDLE HINSTANCE;
  typedef HINSTANCE HMODULE;

  /* 32 bit data types*/
  typedef long INT32;
  typedef unsigned long UINT32;
  typedef unsigned long DWORD;

  typedef long long INT64;
  typedef unsigned long long UINT64;
  typedef BYTE BOOLEAN;

  typedef CHAR *PCHAR;
  typedef CHAR *LPCH, *PCH;

  typedef CONST CHAR *LPCCH, *PCCH;
  typedef CHAR *NPSTR;
  typedef CHAR *LPSTR, *PSTR;
  typedef CONST CHAR *LPCSTR, *PCSTR;

  typedef LPSTR LPTCH, PTCH;
  typedef LPSTR PTSTR, LPTSTR;
  typedef LPCSTR LPCTSTR;

#if defined(_WIN64)
  typedef unsigned __int64 ULONG_PTR;
#else
  typedef unsigned long ULONG_PTR;
#endif

  typedef ULONG_PTR DWORD_PTR;
  typedef ULONG_PTR *PULONG_PTR;

  typedef	unsigned long long u_quad_t;	/* quads */
  typedef	long long	quad_t;
  typedef	quad_t *	qaddr_t;
 

//20180124	
  #define offsetof(st, m) ((size_t)&(((st *)0)->m))
  #define HUGE_VAL 10000000
  #define DBL_MAX_10_EXP                +308

  #ifndef NULL
      #define NULL 0
  #endif

  #define __FLAT__ 1
//  #define FAR far
  #define NEAR near
  #define PASCAL pascal
  #define CDECL cdecl

  #define SUCCESS 1
  #define FAILURE -1

  #define FALSE 0
  #define TRUE  1

  typedef void *LPVOID;
  typedef DWORD *LPDWORD;
  #define WINAPI __stdcall 

  typedef DWORD(WINAPI *LPTHREAD_START_ROUTINE) (LPVOID lpThreadParameter);