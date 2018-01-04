#pragma once
#include "stddef.h"

#ifndef _FILE_DEFINED
#define _FILE_DEFINED
typedef struct _iobuf
{
	void* _Placeholder;
} FILE;
#endif
#define EOF -1

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fflush(FILE *stream);
FILE *freopen(const char *filename, const char *mode, FILE *stream);
int fseek(FILE *stream, long int offset, int whence);
long int ftell(FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int getc(FILE * stream);

char *	strerror(int errnum);

int fprintf(FILE * stream, const char * format, ...);

FILE* __cdecl __acrt_iob_func(unsigned _Ix);

#define stdin  (__acrt_iob_func(0))
#define stdout  (__acrt_iob_func(0))


extern struct FILE *stderr;                /* Standard error output stream.  */

#define stderr stderr

#define NULL 0

										   /* flags for mmioSeek() */
#ifndef SEEK_SET
#define SEEK_SET        0               /* seek to an absolute position */
#define SEEK_CUR        1               /* seek relative to current position */
#define SEEK_END        2               /* seek relative to end of file */
#endif  /* ifndef SEEK_SET */

#define _IOFBF 0x0000
#define _IOLBF 0x0040
#define _IONBF 0x0004

#define HUGE_VAL 10000000