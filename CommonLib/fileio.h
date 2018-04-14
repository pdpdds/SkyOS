#pragma once
#include "windef.h"
#include "stddef.h"
#define SEEK_SET        0               /* seek to an absolute position */
#define SEEK_CUR        1               /* seek relative to current position */
#define SEEK_END        2               /* seek relative to end of file */

//파일 플래그
#define FS_FILE       0
#define FS_DIRECTORY  1
#define FS_INVALID    2

typedef struct _FILE {

	char     _name[32];
	DWORD	 _flags;
	DWORD    _fileLength;
	DWORD    _id;
	DWORD    _eof;
	DWORD    _position;
	DWORD    _currentCluster;
	DWORD    _deviceID;

}FILE, *PFILE;

extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern FILE *fopen(const char *filename, const char *mode);
extern size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
extern int fclose(FILE *stream);
extern int feof(FILE *stream);
extern int ferror(FILE *stream);
extern int fflush(FILE *stream);
extern FILE *freopen(const char *filename, const char *mode, FILE *stream);
extern int fseek(FILE *stream, long int offset, int whence);
extern long int ftell(FILE *stream);
extern int getc(FILE * stream);
extern char*	strerror(int errnum);
extern int fprintf(FILE * stream, const char * format, ...);