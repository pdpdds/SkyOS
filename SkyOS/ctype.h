#pragma once

//****************************************************************************
//**
//**    ctype.h
//**    - character macros
//**
//****************************************************************************
//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

#ifdef _MSC_VER
// Get rid of conversion warnings
#pragma warning (disable:4244)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define isspace(c)      ((c) == ' ' || ((c) >= '\t' && (c) <= '\r'))
#define isascii(c)      (((c) & ~0x7f) == 0)
#define isupper(c)      ((c) >= 'A' && (c) <= 'Z')
#define islower(c)      ((c) >= 'a' && (c) <= 'z')
#define isalpha(c)      (isupper(c) || islower(c))
#define isdigit(c)      ((c) >= '0' && (c) <= '9')
#define isxdigit(c)     (isdigit(c) \
                         || ((c) >= 'A' && (c) <= 'F') \
                         || ((c) >= 'a' && (c) <= 'f'))
#define isprint(c)      ((c) >= ' ' && (c) <= '~')
#define toupper(c)      ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))
#define tolower(c)      ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))
#define toascii(c)	((unsigned)(c) & 0x7F)

#if 0

extern char _ctype[];

/* Constants */

#define _LEADBYTE       0x8000                      // Multibyte leadbyte
#define _ALPHA          (0x0100 | _UPPER| _LOWER)   // Alphabetic character

#define CT_UP	0x01	/* upper case */
#define CT_LOW	0x02	/* lower case */
#define CT_DIG	0x04	/* digit */
#define CT_CTL	0x08	/* control */
#define CT_PUN	0x10	/* punctuation */
#define CT_WHT	0x20	/* white space (space/cr/lf/tab) */
#define CT_HEX	0x40	/* hex digit */
#define CT_SP	0x80	/* hard space (0x20) */

/* Basic macros */

#define isalnum(c)	((_ctype + 1)[(unsigned)(c)] & (CT_UP | CT_LOW | CT_DIG))
#define isalpha(c)	((_ctype + 1)[(unsigned)(c)] & (CT_UP | CT_LOW))
#define iscntrl(c)	((_ctype + 1)[(unsigned)(c)] & (CT_CTL))
#define isdigit(c)	((_ctype + 1)[(unsigned)(c)] & (CT_DIG))
#define isgraph(c)	((_ctype + 1)[(unsigned)(c)] & (CT_PUN | CT_UP | CT_LOW | CT_DIG))
#define islower(c)	((_ctype + 1)[(unsigned)(c)] & (CT_LOW))
#define isprint(c)	((_ctype + 1)[(unsigned)(c)] & (CT_PUN | CT_UP | CT_LOW | CT_DIG | CT_SP))
#define ispunct(c)	((_ctype + 1)[(unsigned)(c)] & (CT_PUN))
#define isspace(c)	((_ctype + 1)[(unsigned)(c)] & (CT_WHT))
#define isupper(c)	((_ctype + 1)[(unsigned)(c)] & (CT_UP))
#define isxdigit(c)	((_ctype + 1)[(unsigned)(c)] & (CT_DIG | CT_HEX))


#endif

#ifdef __cplusplus
}
#endif

//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END ctype.h
//**
//****************************************************************************
