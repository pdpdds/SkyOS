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

	char     _name[32]; //파일 이름
	DWORD	 _flags; //플래그
	DWORD    _fileLength; //파일 길이
	DWORD    _id; //아이디, 또는 핸들
	DWORD    _eof; //파일의 끝에 도달했는가
	DWORD    _position; // 현재 위치
	DWORD    _currentCluster; //현재 클러스터 위치
	DWORD    _deviceID; //디바이스 아이디

}FILE, *PFILE;

#ifdef  __cplusplus
extern "C" {
#endif

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
	extern int fgetc(FILE * stream);
	extern char* fgets(char *dst, int max, FILE *fp);
	int fscanf(FILE *fp, const char *fmt, ...);
	extern char*	strerr(int errnum);
	//extern FILE*	stderr;
	extern int fprintf(FILE * stream, const char * format, ...);
	char *	strerror(int errnum);

#ifdef  __cplusplus
}
#endif


