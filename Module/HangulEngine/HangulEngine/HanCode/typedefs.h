/*
 *      File TYPEDEFS.H
 *      Type Definitions
 *      '92.2.5
 *      '95.5.2
 *      '96.1.8
 *      Written by Inkeon Lim
 *
 *      Modified by fontutil ('07.7.28)
 */


#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_


/** Macro constants ********************************************************/

#ifndef NOERROR
    #define NOERROR 0     /* No error occured      */
    #define ERROR   (-1)  /* Generic error occured */
#endif

#ifdef __cplusplus
    #define __CPPARGS ...
#else
    #define __CPPARGS
#endif


/** Type definitions *******************************************************/

typedef signed char shortbool;
#ifndef __cplusplus
typedef enum {false, true} bool;
#endif

#ifdef __cplusplus
    typedef char hchar;
#else
    typedef unsigned char hchar;
#endif

typedef unsigned char uchar;

typedef unsigned char byte;

typedef unsigned short word;
typedef unsigned long longword;

#ifndef _SIZE_T
    #define _SIZE_T
    typedef unsigned size_t;
#endif

#endif
