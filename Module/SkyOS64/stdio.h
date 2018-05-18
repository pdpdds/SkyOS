
#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#ifdef  __cplusplus
extern "C" {
#endif
extern int vprintf(const char *format, va_list ap);
//extern int vfprintf(FILE *stream, const char *format, va_list ap);
extern size_t vsprintf(char *str, const char *format, va_list ap);
//extern int vsnprintf(char *str, size_t size, const char *format, va_list ap);
//extern int vasprintf(char **ret, const char *format, va_list ap);
extern long strtol(const char* nptr, char** endptr, int base);
extern unsigned long strtoul(const char* nptr, char** endptr, int base);
extern int atoi ( const char * str );
extern int atob(int *vp, char *p, int base);
int      sscanf(const char *, const char *, ...);
#ifdef  __cplusplus
}
#endif
#endif
