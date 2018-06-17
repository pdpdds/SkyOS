
#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#ifdef  __cplusplus
extern "C" {
#endif
extern int vprintf(const char *format, va_list ap);
//extern int vfprintf(FILE *stream, const char *format, va_list ap);
extern int vsprintf(char *str, const char *format, va_list ap);
//extern int vasprintf(char **ret, const char *format, va_list ap);
extern long strtol(const char* nptr, char** endptr, int base);
extern unsigned long strtoul(const char* nptr, char** endptr, int base);
extern int atoi ( const char * str );
extern double atof(char *p);
extern int atob(int *vp, char *p, int base);
int      sscanf(const char *, const char *, ...);

unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base);
long simple_strtol(const char *cp, char **endp, unsigned int base);
unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base);
long long simple_strtoll(const char *cp, char **endp, unsigned int base);
#ifdef  __cplusplus
}
#endif
#endif
